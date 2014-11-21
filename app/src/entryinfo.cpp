#include "entryinfo.h"

EntryInfo::EntryInfo(QObject *parent) :
    QObject(parent)
{
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
