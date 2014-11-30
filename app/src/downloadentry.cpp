#include "downloadentry.h"

DownloadEntry::DownloadEntry(QObject *parent, QString name, QString remotePath, QString localPath, qint64 size) :
    QObject(parent)
{
    m_name = name;
    m_remotePath = remotePath;
    m_localPath = localPath;
    m_size = size;
    m_progress = 0.0;
}

QString DownloadEntry::getName()
{
    return m_name;
}

QString DownloadEntry::getLocalPath()
{
    return m_localPath;
}

QString DownloadEntry::getRemotePath()
{
    return m_remotePath;
}

qint64 DownloadEntry::getSize()
{
    return m_size;
}

qreal DownloadEntry::getProgress()
{
    return m_progress;
}

void DownloadEntry::setProgress(qreal value)
{
    m_progress = value;
    emit progressChanged();
}

void DownloadEntry::startDownload()
{

}

void DownloadEntry::pauseDownload()
{

}

void DownloadEntry::cancelDownload()
{

}
