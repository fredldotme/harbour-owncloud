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
    if (m_existingFiles.contains(relativeLocalPath(filePath))) {
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
        m_existingFiles.insert(relativeLocalPath(entry->localPath()));
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
        qDebug() << Q_FUNC_INFO << "item path:" << item.path();
        if (item.isDir()) {
            m_existingDirs.insert(item.path());
            m_dirsToFetch.append(item.path());
        } else {
            m_existingFiles.insert(relativeRemotePath(item.path()));
        }
    }

    if (m_dirsToFetch.isEmpty()) {
        m_fetchedExisting = true;
        uploadFile();
    } else {
        m_remoteDir.listDirectory(&m_connection, m_dirsToFetch.takeFirst());
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
    do {
        if (m_uploadQueue.isEmpty()) {
            qDebug() << Q_FUNC_INFO << "no files to upload";
            return;
        }

        absolutePath = m_uploadQueue.takeFirst();
        relativePath = relativeLocalPath(absolutePath);
    } while (m_existingFiles.contains(relativePath));

    QFileInfo fileInfo(absolutePath);
    QString path = m_remotePath + relativePath.left(relativePath.length() - fileInfo.fileName().length());
    QStringList requiredDirs;

    if (!m_existingDirs.contains(path)) {
        requiredDirs = path.split("/", QString::SkipEmptyParts);
    }
    QString curPath = "/";
    QStringList dirsToCreate;
    for (int i=0; i<requiredDirs.length(); i++) {
        curPath += requiredDirs[i] + "/";
        if (!m_existingDirs.contains(curPath)) {
            dirsToCreate.append(curPath);
        }
    }

    m_uploading = true;
    UploadEntry *entry = new UploadEntry(absolutePath, relativeToRemote(relativePath), dirsToCreate, &m_connection);
    connect(entry, SIGNAL(finished()), SLOT(uploadFinished()));
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
            emit connectError(reply->errorString());
            return;
        }

        m_existingDirs.insert(m_remotePath);
        // Then list the existing files and folders in it
        m_remoteDir.listDirectory(&m_connection, m_remotePath);
    });
}

QString Uploader::relativeLocalPath(QString absolutePath)
{
    return absolutePath.right(absolutePath.length() - Settings::instance()->localPicturesPath().length());
}

QString Uploader::relativeRemotePath(QString absolutePath)
{
    return absolutePath.right(absolutePath.length() - m_remotePath.length());
}
