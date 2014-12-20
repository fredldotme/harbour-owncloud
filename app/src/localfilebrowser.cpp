#include "localfilebrowser.h"

LocalFileBrowser::LocalFileBrowser(QObject *parent) :
    QObject(parent)
{
}

QVariantList LocalFileBrowser::cd()
{
    return cd(QStandardPaths::writableLocation(QStandardPaths::HomeLocation));
}

QString LocalFileBrowser::path()
{
    return m_path;
}

void LocalFileBrowser::setPath(QString value)
{
    m_path = value;
}

QVariantList LocalFileBrowser::cd(QString path)
{
    QDir dir(path);
    m_path = dir.absolutePath();
    QVariantList ret;
    foreach (QFileInfo info, dir.entryInfoList(QDir::NoFilter, QDir::DirsFirst | QDir::Name)) {
        if(info.fileName() == ".." &&
                m_path == QStandardPaths::writableLocation(QStandardPaths::HomeLocation)) {
            continue;
        }

        if(info.fileName() != ".") {
            EntryInfo *entry = new EntryInfo();
            entry->setDirectory(info.isDir());
            entry->setName(info.fileName());
            entry->setPath(info.absoluteFilePath());

            QVariant variant;
            variant.setValue(entry);
            ret.append(variant);
        }
    }
    return ret;
}
