#include "uploadentry.h"
#define DEBUG_WEBDAV 1
#include "qwebdav.h"

UploadEntry::UploadEntry(QString localPath,
                         QString remotePath,
                         QStringList pathsToCreate,
                         QWebdav *connection,
                         QObject *parent) : QObject(parent),
    m_connection(connection),
    m_pathsToCreate(pathsToCreate),
    m_localPath(localPath),
    m_remotePath(remotePath),
    m_succeeded(true),
    m_currentReply(0)
{
    doUpload();
}

UploadEntry::~UploadEntry()
{

}

void UploadEntry::resetReply()
{
    m_currentReply = 0;
}

void UploadEntry::doUpload()
{
    if (!m_pathsToCreate.isEmpty()) {
        qDebug() << Q_FUNC_INFO << "need to create dirs:" << m_pathsToCreate;
        m_createdPaths.append(m_pathsToCreate.at(0));
        createDirectory();
        return;
    }

    QFile *file = new QFile(m_localPath, this);
    if(!file->open(QIODevice::ReadOnly)) {
        emit uploadFailed(tr("Failed to open file %1 for reading").arg(m_localPath));
        emit finished();
    }
    QNetworkReply *reply = m_connection->put(m_remotePath, file);
    m_currentReply = reply;
    connect(reply, &QNetworkReply::finished, this, &UploadEntry::finished);
    connect(reply, static_cast<void(QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error), this, &UploadEntry::errorHandler);
    connect(reply, &QNetworkReply::finished, this, &UploadEntry::resetReply, Qt::DirectConnection);
    connect(reply, &QNetworkReply::finished, reply, &QObject::deleteLater, Qt::DirectConnection);
}

void UploadEntry::errorHandler(QNetworkReply::NetworkError error)
{
    if (error == QNetworkReply::NoError) {
        qDebug() << Q_FUNC_INFO << "error handler got no error";
        return;
    }

    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    Q_ASSERT(reply);
    if(!reply->errorString().endsWith("Method Not Allowed")) {
        emit uploadFailed(reply->errorString());
        qWarning() << Q_FUNC_INFO << "error during upload:" << reply->errorString();
        m_succeeded = false;
    } else {
        qDebug() << "The error code is: " << reply->error();
    }
}

void UploadEntry::createDirectory()
{
    QNetworkReply *reply = m_connection->mkdir(m_pathsToCreate.takeFirst());
    m_currentReply = reply;
    connect(reply, &QNetworkReply::finished, this, &UploadEntry::doUpload);
    connect(reply, static_cast<void(QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error), this, &UploadEntry::errorHandler);
    connect(reply, &QNetworkReply::finished, this, &UploadEntry::resetReply, Qt::DirectConnection);
    connect(reply, &QNetworkReply::finished, reply, &QObject::deleteLater, Qt::DirectConnection);
}

