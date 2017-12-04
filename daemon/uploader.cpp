#define DEBUG_WEBDAV 1
#include "uploader.h"
#include "nextcloudsettings.h"

#include <QNetworkReply>

Uploader::Uploader(QObject *parent) : QObject(parent),
    m_currentReply(0),
    m_currentEntry(0),
    m_uploading(false),
    m_fetchedExisting(false),
    m_online(false)
{
    setRemoteDirectory();
    applySettings();

    connect(&m_remoteDir, &QWebdavDirParser::finished, this, &Uploader::remoteListingFinished);
    connect(&m_remoteDir, &QWebdavDirParser::errorChanged, [](QString error) {
        qDebug() <<  "dir parser error:" << error;
    });
    connect(&m_connection, &QWebdav::errorChanged, [](QString error) {
        qDebug() << "webdav error:" << error;
    });
}


Uploader *Uploader::instance()
{
    static Uploader uploader;
    return &uploader;
}

void Uploader::setRemoteDirectory()
{
    QFile hwRelease("/etc/hw-release");
    if(hwRelease.open(QFile::ReadOnly)) {
        QString allLines = QString(hwRelease.readAll());
        hwRelease.close();
        QStringList lineArray = allLines.split("\n");
        foreach(QString line, lineArray) {
            if(line.startsWith("NAME=")) {
                m_remotePath = "/" + line.replace("NAME=", "").replace("\"", "").trimmed() + "/";
                return;
            }
        }
    }
    m_remotePath = "/Jolla/";
}

void Uploader::fileFound(QString filePath)
{
    QString remoteEquivalent = relativeLocalPath(filePath);
    remoteEquivalent =  m_remotePath + remoteEquivalent.right(remoteEquivalent.length() - 1);
    if (m_existingFiles.contains(remoteEquivalent)) {
        return;
    }

    if(!m_uploadQueue.contains(filePath)) {
        qDebug() << Q_FUNC_INFO << "adding file to upload" << filePath;
        m_uploadQueue.append(filePath);
    }

    if(!m_uploading && m_fetchedExisting)
        uploadFile();
}

void Uploader::setOnline(bool online)
{
    m_online = online;
    settingsChanged();
}

void Uploader::abort()
{
    m_fetchedExisting = false;
    if(m_currentReply)
        m_currentReply->abort();
    if(m_currentEntry)
        m_currentEntry->cancelTransfer();
    m_remoteDir.abort();
    m_existingDirs.clear();
    m_existingFiles.clear();
    m_dirsToFetch.clear();
    m_uploadQueue.clear();
}

void Uploader::applySettings()
{
    NextcloudSettings *settings = NextcloudSettings::instance();
    m_connection.setConnectionSettings(settings->isHttps() ? QWebdav::HTTPS : QWebdav::HTTP,
                                       settings->hostname(),
                                       settings->path() + "remote.php/webdav",
                                       settings->username(),
                                       settings->password(),
                                       settings->port(),
                                       settings->isHttps() ? settings->md5Hex() : "",
                                       settings->isHttps() ? settings->sha1Hex() : "");
}

void Uploader::settingsChanged()
{
    abort();
    applySettings();
    if(m_online)
        getExistingRemote();
}

void Uploader::uploadFinished()
{
    qDebug() << Q_FUNC_INFO;
    m_uploading = false;
    TransferEntry *entry = qobject_cast<TransferEntry*>(sender());
    Q_ASSERT(entry);
    if (entry->succeeded()) {
        qDebug() << Q_FUNC_INFO << "remote path: " << entry->getRemotePath();
        m_existingFiles.insert(entry->getRemotePath());
        foreach(QString createdPath, entry->getCreatedPaths()) {
            qDebug() << Q_FUNC_INFO << "created path: " << createdPath;
            m_existingDirs.insert(createdPath);
        }

        emit fileUploaded(entry->getLocalPath());
    } else {
        qDebug() << Q_FUNC_INFO << "NOT SUCCEEDED!" << entry->getLocalPath();
        m_uploadQueue.append(entry->getLocalPath());
    }
    m_currentEntry = 0;
    uploadFile();
}

void Uploader::remoteListingFinished()
{
    foreach(const QWebdavItem item, m_remoteDir.getList()) {
        if (item.isDir()) {
            if(!m_existingDirs.contains(item.path()))
                m_dirsToFetch.append(item.path());
        } else {
            m_existingFiles.insert(item.path());
        }
    }

    if (m_dirsToFetch.isEmpty()) {
        m_fetchedExisting = true;

        qDebug() << "Existing:";
        foreach(QString tmp, m_existingFiles)
        {
            qDebug() << tmp;
        }
        foreach(QString tmp, m_existingDirs)
        {
            qDebug() << tmp;
        }

        qDebug() << "poking filesystem scanner";
        emit pokeFilesystemScanner();
    } else {
        QString tmp = m_dirsToFetch.takeFirst();
        if(!m_existingDirs.contains(tmp)) {
            m_existingDirs.insert(tmp);
            m_remoteDir.listDirectory(&m_connection, tmp);
        }
    }
}

void Uploader::uploadFile()
{
    if (!m_online) {
        qDebug() << Q_FUNC_INFO << "not online, abort";
        return;
    }

    if (m_uploading) {
        qDebug() << Q_FUNC_INFO << "already uploading";
        return;
    }

    if (!m_fetchedExisting) {
        qDebug() << Q_FUNC_INFO << "haven't fetched existing files";
        return;
    }

    QString absolutePath; // local path with file name in it
    QString relativePath;
    QString path; // remote path with file name in it
    do {
        if (m_uploadQueue.isEmpty()) {
            qDebug() << Q_FUNC_INFO << "no files to upload";
            emit uploadingChanged(false);
            return;
        }

        absolutePath = m_uploadQueue.takeFirst();
        relativePath = relativeLocalPath(absolutePath);
        path = m_remotePath + relativePath.right(relativePath.length() - 1);
    } while (m_existingFiles.contains(path));

    qDebug() << "destination: " << path;
    QStringList requiredDirs;

    if (!m_existingDirs.contains(path)) {
        requiredDirs = path.split("/", QString::SkipEmptyParts);
    }
    QString curPath = "/";
    QStringList dirsToCreate;
    for (int i=0; i<requiredDirs.length() - 1; i++) {
        curPath += requiredDirs[i] + "/";
        if (!m_existingDirs.contains(curPath)) {
            qDebug() << "got to create: |" << curPath << "|";
            dirsToCreate.append(curPath);
        }
    }

    if(!path.endsWith("/") && !m_existingFiles.contains(path)) {
        qDebug() << "Uploading path " << path << " (absolutepath " << absolutePath << ")";

        m_uploading = true;
        emit uploadingChanged(true);

        // Abuse QFileInfo to make this easy
        QString remoteDir = QFileInfo(QFile(path)).dir().path() + QString("/");
        QFileInfo localFileInfo = QFileInfo(QFile(absolutePath));

        bool open = false;
        m_currentEntry = new TransferEntry(this,
                                           &m_connection,
                                           localFileInfo.fileName(),
                                           remoteDir,
                                           absolutePath,
                                           localFileInfo.size(),
                                           TransferEntry::TransferDirection::UP,
                                           open,
                                           dirsToCreate);
        m_currentEntry->startTransfer();
        connect(m_currentEntry, &TransferEntry::transferCompleted, this, &Uploader::uploadFinished);
        connect(m_currentEntry, &TransferEntry::transferCompleted, m_currentEntry, &TransferEntry::deleteLater);
    }
}

QString Uploader::relativeToRemote(QString path)
{
    return NextcloudSettings::instance()->path() + m_remotePath + path;
}

void Uploader::getExistingRemote()
{
    if(!m_online)
        return;

    // Ensure that our remote directory is created
    QNetworkReply *reply = m_connection.mkdir(m_remotePath);
    m_currentReply = reply;
    connect(reply, &QNetworkReply::finished, [this, reply]() {
        if (reply->error() != QNetworkReply::NoError) {
            qWarning() << Q_FUNC_INFO << reply->errorString();
            // don't fail if directory exists
            if(!reply->errorString().endsWith("Method Not Allowed")) {
                qDebug() << "Error: " << reply->errorString();
                emit connectError(reply->errorString());
                return;
            } else {
                qDebug() << "The error code is: " << reply->error();
            }
        }

        m_existingDirs.insert(m_remotePath);
        // Then list the existing files and folders in it
        m_remoteDir.listDirectory(&m_connection, m_remotePath);
    });
    connect(reply, &QNetworkReply::finished, this, &Uploader::resetReply, Qt::DirectConnection);
    connect(reply, &QNetworkReply::finished, reply, &QObject::deleteLater, Qt::DirectConnection);
}

void Uploader::resetReply()
{
    m_currentReply = 0;
}

QString Uploader::relativeLocalPath(QString absolutePath)
{
    return absolutePath.right(absolutePath.length() - NextcloudSettings::instance()->localPicturesPath().length());
}

QString Uploader::relativeRemotePath(QString absolutePath)
{
    return absolutePath.right(absolutePath.length() - m_remotePath.length());
}
