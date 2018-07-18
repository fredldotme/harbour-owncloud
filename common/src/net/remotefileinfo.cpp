#include "remotefileinfo.h"

RemoteFileInfo::RemoteFileInfo(QObject *parent) :
    QObject(parent)
{
}

QString RemoteFileInfo::path()
{
    return m_path;
}

void RemoteFileInfo::setPath(QString value)
{
    m_path = value;
}

QString RemoteFileInfo::name()
{
    return m_name;
}

void RemoteFileInfo::setName(QString value)
{
    m_name = value;
}

bool RemoteFileInfo::isDirectory()
{
    return m_isDirectory;
}

void RemoteFileInfo::setDirectory(bool value)
{
    m_isDirectory = value;
}

qint64 RemoteFileInfo::size()
{
    return m_size;
}

void RemoteFileInfo::setSize(qint64 value)
{
    m_size = value;
}

void RemoteFileInfo::setMimeType(QString value)
{
    m_mime = value;
}

QString RemoteFileInfo::mimeType()
{
    return m_mime;
}

void RemoteFileInfo::setModTime(QDateTime modTime)
{
    m_modTime = modTime;
}

QDateTime RemoteFileInfo::modTime()
{
    return m_modTime;
}
