#include "transferentry.h"

TransferEntry::TransferEntry(QObject *parent, QWebdav *webdav,
                             QString name, QString remotePath,
                             QString localPath, qint64 size,
                             TransferDirection direction, bool open,
                             QStringList pathsToCreate) :
    QObject(parent)
{
    this->webdav = webdav;
    webdav->setParent(this);
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

void TransferEntry::startTransfer()
{
    qDebug() << "Start transfer";
    qDebug() << "Local path: " << m_localPath;
    qDebug() << "Remote path: " << m_remotePath;

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
}

void TransferEntry::handleProgressChange(qint64 bytes, qint64 bytesTotal)
{
    if(bytesTotal > 0)
        setProgress((qreal)bytes/(qreal)bytesTotal);
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
