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
    m_succeeded(true)
{
    doUpload();
}

UploadEntry::~UploadEntry()
{

}

void UploadEntry::doUpload()
{
    if (!m_pathsToCreate.isEmpty()) {
        qDebug() << Q_FUNC_INFO << "need to create dirs:" << m_pathsToCreate;
        createDirectory();
        return;
    }

    QFile *file = new QFile(m_localPath, this);
    if(!file->open(QIODevice::ReadOnly)) {
        emit uploadFailed(tr("Failed to open file %1 for reading").arg(m_localPath));
        emit finished();
    }
    QNetworkReply *reply = m_connection->put(m_remotePath, file);
    connect(reply, SIGNAL(finished()), SIGNAL(finished()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), SLOT(errorHandler(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(finished()), reply, SLOT(deleteLater()));
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
    }
}

void UploadEntry::createDirectory()
{
    QNetworkReply *reply = m_connection->mkdir(m_pathsToCreate.at(0));
    connect(reply, SIGNAL(finished()), SLOT(doUpload()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), SLOT(errorHandler(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(finished()), reply, SLOT(deleteLater()));
}

