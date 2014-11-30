#include "downloadentry.h"

DownloadEntry::DownloadEntry(QObject *parent, QWebdav *webdav, QString name, QString remotePath, QString localPath, qint64 size, bool open) :
    QObject(parent)
{
    this->webdav = webdav;

    m_name = name;
    m_remotePath = remotePath;
    m_localPath = localPath;
    m_size = size;
    m_progress = 0.0;

    m_open = open;
}

DownloadEntry::~DownloadEntry()
{
    disconnect(webdav, SIGNAL(finished(QNetworkReply*)), this, SLOT(handleReadComplete()));

    if(localFile)
        delete localFile;
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
    localFile = new QFile(m_localPath);
    localFile->open(QFile::ReadWrite);
    connect(webdav, SIGNAL(finished(QNetworkReply*)), this, SLOT(handleReadComplete()));
    webdav->get(m_remotePath, localFile);
}

void DownloadEntry::pauseDownload()
{

}

void DownloadEntry::cancelDownload()
{

}

void DownloadEntry::handleReadComplete()
{
    if(m_open) {
        ShellCommand::runCommand("xdg-open", QStringList(getLocalPath()));
    }
    emit downloadCompleted();
}
