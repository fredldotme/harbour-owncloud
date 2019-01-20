#include "localfilebrowser.h"

#include <QDateTime>
#include <QDebug>
#include <QStorageInfo>

LocalFileBrowser::LocalFileBrowser(QObject *parent) :
    QObject(parent)
{
    cd();
}

void LocalFileBrowser::cd()
{
    cd(QStandardPaths::writableLocation(QStandardPaths::HomeLocation),
              QStandardPaths::writableLocation(QStandardPaths::HomeLocation));
}

void LocalFileBrowser::cd(QString newPath)
{
    cd(newPath, pathRestriction());
}

QVariantList LocalFileBrowser::mountedVolumes()
{
    const QStringList allowedMountPoints {
        QStringLiteral("/run/media/"), QStringLiteral("/media/"), QStringLiteral("/mnt/"),
    };

    QVariantList ret;

    // Manually add home directory
    {
        QVariantMap homeEntry;
        homeEntry.insert(QStringLiteral("displayName"),
                         QStandardPaths::displayName(QStandardPaths::HomeLocation));
        homeEntry.insert(QStringLiteral("name"),
                         QStandardPaths::displayName(QStandardPaths::HomeLocation));
        homeEntry.insert(QStringLiteral("rootPath"),
                         QStandardPaths::writableLocation(QStandardPaths::HomeLocation));
        ret.append(homeEntry);
    }

    for (const QStorageInfo& volume : QStorageInfo::mountedVolumes()) {
        if (!volume.isValid() || volume.isRoot())
            continue;

        bool allowed = false;
        for (const QString allowedMountPoint : allowedMountPoints) {
            if (volume.rootPath().startsWith(allowedMountPoint)) {
                allowed = true;
                break;
            }
        }
        if (!allowed)
            continue;

        QVariantMap entry;
        entry.insert(QStringLiteral("displayName"), volume.displayName());
        entry.insert(QStringLiteral("name"), volume.name());
        entry.insert(QStringLiteral("rootPath"), volume.rootPath());
        entry.insert(QStringLiteral("device"), volume.device());
        entry.insert(QStringLiteral("fileSystemType"), volume.fileSystemType());
        entry.insert(QStringLiteral("isReadOnly"), volume.isReadOnly());
        entry.insert(QStringLiteral("isReady"), volume.isReady());
        entry.insert(QStringLiteral("blockSize"), volume.blockSize());
        entry.insert(QStringLiteral("bytesAvailable"), volume.bytesAvailable());
        entry.insert(QStringLiteral("bytesFree"), volume.bytesFree());
        entry.insert(QStringLiteral("bytesTotal"), volume.bytesTotal());

        ret.append(entry);
    }
    return ret;
}

QString LocalFileBrowser::path()
{
    return m_path;
}

void LocalFileBrowser::setPath(QString value)
{
    if (this->m_path == value)
        return;

    this->m_path = value;
    Q_EMIT pathChanged();
}

QString LocalFileBrowser::pathRestriction()
{
    return m_pathRestriction;
}

void LocalFileBrowser::setPathRestriction(QString value)
{
    if (this->m_pathRestriction == value)
        return;

    this->m_pathRestriction = value;
    Q_EMIT pathRestrictionChanged();
}

QVariantList LocalFileBrowser::content()
{
    return this->m_content;
}

void LocalFileBrowser::setContent(QVariantList content)
{
    if (this->m_content == content)
        return;

    this->m_content = content;
    Q_EMIT contentChanged();
}

void LocalFileBrowser::cd(QString newPath, QString pathRestriction)
{
    QVariantList dirContent;

    const QDir dir(newPath);
    if (!dir.isReadable()) {
        Q_EMIT errorOccured("Permission denied");
        return;
    }

    const QDir restrictionDir(pathRestriction);
    setPathRestriction(pathRestriction);


    const QFileInfoList newPathContent =
            dir.entryInfoList(QDir::NoFilter, QDir::Name | QDir::DirsFirst);

    for (const QFileInfo& info : newPathContent) {
        if (info.fileName() == QStringLiteral("..") &&
                dir.absolutePath() == restrictionDir.absolutePath())
            continue;

        if (info.fileName() == QStringLiteral("."))
            continue;

        QVariantMap entry;

        entry.insert(QStringLiteral("isDirectory"), info.isDir());
        entry.insert(QStringLiteral("name"), info.fileName());
        entry.insert(QStringLiteral("path"), info.absoluteFilePath());
        entry.insert(QStringLiteral("lastModified"), info.lastModified());
        entry.insert(QStringLiteral("created"), info.created());

        dirContent.append(entry);
    }

    setPath(dir.absolutePath());
    setContent(dirContent);
}
