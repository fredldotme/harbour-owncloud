#include "transferentry.h"

TransferEntry::TransferEntry(QObject *parent, QWebdav *webdav,
                             QString name, QString remotePath,
                             QString localPath, qint64 size,
                             TransferDirection direction, bool open,
                             QStringList pathsToCreate) :
    QObject(parent),
    m_pathsToCreate(pathsToCreate)
{
    this->webdav = webdav;
    // XXX: With this call, the daemon dies on an invalid pointer free
    // XXX: Probably due to passing &m_connection, need to figure out
    //      if a new webdav instance would help in uploader.cpp
    //webdav->setParent(this);
    this->networkReply = 0;

    m_name = name;
    m_remotePath = remotePath;
    m_localPath = localPath;
    m_size = size;
    m_progress = 0.0;
    m_direction = direction;
    m_open = open;
}

TransferEntry::~TransferEntry()
{
    disconnect(this, 0, 0, 0);
}

QStringList TransferEntry::getCreatedPaths() {
    return m_createdPaths;
}

QString TransferEntry::getName()
{
    return m_name;
}

QString TransferEntry::getLocalPath()
{
    return m_localPath;
}

QString TransferEntry::getRemotePath()
{
    return m_remotePath;
}

qint64 TransferEntry::getSize()
{
    return m_size;
}

QDateTime TransferEntry::getLastModified()
{
    return m_lastModified;
}

qreal TransferEntry::getProgress()
{
    return m_progress;
}

void TransferEntry::setLastModified(QDateTime lastModified)
{
    this->m_lastModified = lastModified;
}

void TransferEntry::setProgress(qreal value)
{
    if(m_progress != value) {
        m_progress = value;
        emit progressChanged(m_progress, m_remotePath);
    }
}

int TransferEntry::getTransferDirection()
{
    return m_direction;
}

bool TransferEntry::hasPathsToCreate()
{
    // Passing NULL in the constructor creates a QStringList("")
    return !m_pathsToCreate.isEmpty() && m_pathsToCreate != QStringList("");
}

void TransferEntry::startTransfer()
{
    qDebug() << "Start transfer";
    qDebug() << "Local path: " << m_localPath;
    qDebug() << "Remote path: " << m_remotePath;

    if (this->hasPathsToCreate()) {
        qDebug() << Q_FUNC_INFO << "need to create dirs:" << m_pathsToCreate;
        // XXX: This should be in createDirectory after it's actually created
        m_createdPaths.append(m_pathsToCreate.at(0));
        createDirectory();
        return;
    }

    localFile = new QFile(m_localPath, this);

    localFile->open(QFile::ReadWrite);
    connect(webdav, &QNetworkAccessManager::finished, this, &TransferEntry::handleReadComplete);
    if(m_direction == DOWN) {
        qDebug() << "Start dl last modified: " << getLastModified().toString("yyyy-MM-ddThh:mm:ss.zzz+t");
        networkReply = webdav->get(m_remotePath, localFile);
        connect(networkReply, &QNetworkReply::downloadProgress,
                this, &TransferEntry::handleProgressChange, Qt::DirectConnection);
    } else {
        localFileInfo = new QFileInfo(*localFile);
        this->setLastModified(localFileInfo->lastModified());
        qDebug() << "Start up last modified: " << getLastModified().toString("yyyy-MM-ddThh:mm:ss.zzz+t");
        networkReply = webdav->put(m_remotePath + m_name, localFile);
        connect(networkReply, &QNetworkReply::uploadProgress,
                this, &TransferEntry::handleProgressChange, Qt::DirectConnection);
    }
    connect(this, &QObject::destroyed, networkReply, &QObject::deleteLater);
    connect(networkReply, &QObject::destroyed, localFile, &QObject::deleteLater);
    connect(networkReply, &QNetworkReply::finished, this, &TransferEntry::resetReply);
}

void TransferEntry::handleProgressChange(qint64 bytes, qint64 bytesTotal)
{
    if(bytesTotal > 0) {
        setProgress((qreal)bytes/(qreal)bytesTotal);
    } else {
        // An empty file is considered a success if we make it this far
        setProgress((qreal) 1.0);
    }
}

void TransferEntry::cancelTransfer()
{
    if(networkReply) {
        networkReply->abort();
    }

    if(m_direction == DOWN) {
        QFile tmpFile(m_localPath);
        tmpFile.remove();
    }

    emit transferCompleted(false);
}

void TransferEntry::handleReadComplete()
{
    if(m_open) {
        ShellCommand::runCommand("xdg-open", QStringList(getLocalPath()));
    }

    emit transferCompleted(true);
}

bool TransferEntry::succeeded()
{
    return this->getProgress() == 1.0;
}

void TransferEntry::createDirectory()
{
    QString toCreate = m_pathsToCreate.takeFirst();
    Q_ASSERT(toCreate.length() > 0);

    QNetworkReply *reply = webdav->mkdir(toCreate);
    connect(reply, &QNetworkReply::finished, this, &TransferEntry::startTransfer);
    connect(reply, static_cast<void(QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error), this, &TransferEntry::errorHandler);
    connect(reply, &QNetworkReply::finished, reply, &QObject::deleteLater, Qt::DirectConnection);
}

void TransferEntry::errorHandler(QNetworkReply::NetworkError error)
{
    if (error == QNetworkReply::NoError) {
        qDebug() << Q_FUNC_INFO << "error handler got no error";
        return;
    }

    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    Q_ASSERT(reply);
    if(!reply->errorString().endsWith("Method Not Allowed")) {
        emit transferCompleted(false);
        qWarning() << Q_FUNC_INFO << "error during upload:" << reply->errorString();
        setProgress((qreal) 0);
    } else {
        qDebug() << "The error code is: " << reply->error();
    }
}

void TransferEntry::resetReply()
{
    networkReply = 0;
}
