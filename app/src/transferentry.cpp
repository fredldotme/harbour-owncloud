#include "transferentry.h"

TransferEntry::TransferEntry(QObject *parent, QWebdav *webdav,
                             QString name, QString remotePath,
                             QString localPath, qint64 size,
                             TransferDirection direction, bool open) :
    QObject(parent)
{
    this->webdav = webdav;

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
    disconnect(webdav, SIGNAL(finished(QNetworkReply*)), this, SLOT(handleReadComplete()));
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

qreal TransferEntry::getProgress()
{
    return m_progress;
}

void TransferEntry::setProgress(qreal value)
{
    m_progress = value;
    emit progressChanged();
}

void TransferEntry::startTransfer()
{
    localFile = new QFile(m_localPath, this);
    localFile->open(QFile::ReadWrite);
    connect(webdav, SIGNAL(finished(QNetworkReply*)), this, SLOT(handleReadComplete()));
    if(m_direction == DOWN) {
        webdav->get(m_remotePath, localFile);
    } else {
        webdav->put(m_remotePath, localFile);
    }
}

void TransferEntry::pauseTransfer()
{

}

void TransferEntry::cancelTransfer()
{

}

void TransferEntry::handleReadComplete()
{
    if(m_open) {
        ShellCommand::runCommand("xdg-open", QStringList(getLocalPath()));
    }

    emit downloadCompleted();
}
