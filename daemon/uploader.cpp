#define DEBUG_WEBDAV 1
#include "uploader.h"
#include "settings.h"
#include "uploadentry.h"

#include <QNetworkReply>

Uploader::Uploader(QObject *parent) : QObject(parent),
    m_uploading(false),
    m_fetchedExisting(false),
    m_remotePath("/Jolla/"),
    m_suspended()
{
    connect(&m_remoteDir, SIGNAL(finished()), SLOT(remoteListingFinished()));

    connect(&m_remoteDir, &QWebdavDirParser::errorChanged, [](QString error) {
        qDebug() <<  "dir parser error:" << error;
    });
    connect(&m_connection, &QWebdav::errorChanged, [](QString error) {
        qDebug() << "webdav error:" << error;
    });

    QMetaObject::invokeMethod(this, "settingsChanged", Qt::QueuedConnection);
}

void Uploader::fileFound(QString filePath)
{
    QString remoteEquivalent = relativeLocalPath(filePath);
    remoteEquivalent =  m_remotePath + remoteEquivalent.right(remoteEquivalent.length() - 1);
    if (m_existingFiles.contains(remoteEquivalent)) {
        return;
    }

    qDebug() << Q_FUNC_INFO << "adding file to upload" << filePath;
    m_uploadQueue.append(filePath);
    uploadFile();
}

void Uploader::setSuspended(bool suspended)
{
    if (m_suspended == suspended) {
        return;
    }

    m_suspended = suspended;

    if (!suspended) {
        // restart uploading
        uploadFile();
    }
}

void Uploader::settingsChanged()
{
    Settings *settings = Settings::instance();

    m_connection.setConnectionSettings(settings->isHttps() ? QWebdav::HTTPS : QWebdav::HTTP,
                                 settings->hostname(),
                                 settings->path() + "/remote.php/webdav",
                                 settings->username(),
                                 settings->password(),
                                 settings->port(),
                                 settings->md5Hex(),
                                 settings->sha1Hex());

    getExistingRemote();
    emit localPathUpdated();
}

void Uploader::uploadFinished()
{
    qDebug() << Q_FUNC_INFO;
    m_uploading = false;
    UploadEntry *entry = qobject_cast<UploadEntry*>(sender());
    Q_ASSERT(entry);
    if (entry->succeeded()) {
        m_existingFiles.insert(entry->remotePath());
        foreach(QString createdPath, entry->pathsToCreate()) {
            qDebug() << "created path: " << createdPath;
            m_existingDirs.insert(createdPath);
        }

        emit fileUploaded(entry->localPath());
    } else {
        m_uploadQueue.append(entry->localPath());
    }
    entry->deleteLater();
    uploadFile();
}

void Uploader::remoteListingFinished()
{
    foreach(const QWebdavItem item, m_remoteDir.getList()) {
        if (item.isDir()) {
            if(!m_existingDirs.contains("/" + item.path()))
                m_dirsToFetch.append("/" + item.path());
        } else {
            m_existingFiles.insert("/" + item.path());
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

        uploadFile();
    } else {
        QString tmp = m_dirsToFetch.takeFirst();
        if(!m_existingDirs.contains(tmp)) {
            m_existingDirs.insert(tmp);
            m_remoteDir.listDirectory(&m_connection, tmp);
        }
    }
}

void Uploader::onlineChanged(bool online)
{
    setSuspended(online);
}

void Uploader::uploadFile()
{
    if (m_uploading) {
        qDebug() << Q_FUNC_INFO << "already uploading";
        return;
    }

    if (!m_fetchedExisting) {
        qDebug() << Q_FUNC_INFO << "haven't fetched existing files";
        return;
    }

    if (m_suspended) {
        qDebug() << Q_FUNC_INFO << "suspended, not downloading";
    }

    QString absolutePath;
    QString relativePath;
    QString path;
    do {
        if (m_uploadQueue.isEmpty()) {
            qDebug() << Q_FUNC_INFO << "no files to upload";
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
        m_uploading = true;
        UploadEntry *entry = new UploadEntry(absolutePath, path, dirsToCreate, &m_connection);
        connect(entry, SIGNAL(finished()), SLOT(uploadFinished()));
    }
}

QString Uploader::relativeToRemote(QString path)
{
    return Settings::instance()->path() + m_remotePath + path;
}

void Uploader::getExistingRemote()
{
    // Ensure that our remote directory is created
    QNetworkReply *reply = m_connection.mkdir(m_remotePath);
    connect(reply, &QNetworkReply::finished, [this, reply]() {
        if (reply->error() != QNetworkReply::NoError) {
            qWarning() << Q_FUNC_INFO << reply->errorString();
            // don't fail if directory exists
            if(!reply->errorString().endsWith("Method Not Allowed")) {
                emit connectError(reply->errorString());
                return;
            }
        }

        m_existingDirs.insert(m_remotePath);
        // Then list the existing files and folders in it
        m_remoteDir.listDirectory(&m_connection, m_remotePath);
    });
    connect(reply, SIGNAL(finished()), reply, SLOT(deleteLater()));
}

QString Uploader::relativeLocalPath(QString absolutePath)
{
    return absolutePath.right(absolutePath.length() - Settings::instance()->localPicturesPath().length());
}

QString Uploader::relativeRemotePath(QString absolutePath)
{
    return absolutePath.right(absolutePath.length() - m_remotePath.length());
}
