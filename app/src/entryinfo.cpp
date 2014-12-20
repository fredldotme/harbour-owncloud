#include "entryinfo.h"

EntryInfo::EntryInfo(QObject *parent) :
    QObject(parent)
{
}

QString EntryInfo::path()
{
    return m_path;
}

void EntryInfo::setPath(QString value)
{
    m_path = value;
}

QString EntryInfo::name()
{
    return m_name;
}

void EntryInfo::setName(QString value)
{
    m_name = value;
}

bool EntryInfo::isDirectory()
{
    return m_isDirectory;
}

void EntryInfo::setDirectory(bool value)
{
    m_isDirectory = value;
}

qint64 EntryInfo::size()
{
    return m_size;
}

void EntryInfo::setSize(qint64 value)
{
    m_size = value;
}

void EntryInfo::setMimeType(QString value)
{
    m_mime = value;
}

QString EntryInfo::mimeType()
{
    return m_mime;
}

void EntryInfo::setModTime(QDateTime modTime)
{
    m_modTime = modTime;
}

QDateTime EntryInfo::modTime()
{
    return m_modTime;
}
