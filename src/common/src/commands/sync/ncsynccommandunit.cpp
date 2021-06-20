#include "ncsynccommandunit.h"

#include <provider/storage/cloudstorageprovider.h>
#include <commands/webdav/mkdavdircommandentity.h>
#include <commands/webdav/fileuploadcommandentity.h>
#include <commands/webdav/davproppatchcommandentity.h>

#include <QDir>
#include <QFile>
#include <QFileInfo>

static CommandEntity* defaultCommandEntity(QObject* parent,
                                    CloudStorageProvider* client,
                                    QString remotePath,
                                    QSharedPointer<NcDirNode> cachedTree)
{
    qInfo() << Q_FUNC_INFO << remotePath;
    return new NcDirTreeCommandUnit(parent, client, remotePath, cachedTree);
}

static CommandEntityInfo defaultCommandInfo(const QString& localPath, const QString& remotePath)
{
    QVariantMap info;
    info.insert(QStringLiteral("type"), "dirTree");
    info.insert("localPath", localPath);
    info.insert("remotePath", remotePath);
    return CommandEntityInfo(info);
}

NcSyncCommandUnit::NcSyncCommandUnit(QObject* parent,
                                     CloudStorageProvider* client,
                                     QString localPath,
                                     QString remotePath,
                                     QSharedPointer<NcDirNode> cachedTree) :
    CommandUnit(parent, {defaultCommandEntity(parent, client, remotePath, cachedTree)},
                defaultCommandInfo(localPath, remotePath)),
    m_client(client),
    m_localPath(localPath),
    m_remotePath(remotePath),
    m_cachedTree(cachedTree)
{

}

QSharedPointer<NcDirNode> NcSyncCommandUnit::cachedTree()
{
    return this->m_cachedTree;
}

bool NcSyncCommandUnit::fileExistsRemotely(const QString& localFilePath,
                                           QStringList& missingDirectories)
{
    const QString relativeFilePath = localFilePath.mid(this->m_localPath.length());
#if QT_VERSION < 0x060000
    QStringList pathCrumbs = relativeFilePath.split("/", QString::SkipEmptyParts);
#else
    QStringList pathCrumbs = relativeFilePath.split("/", Qt::SkipEmptyParts);
#endif
    const QString fileName = pathCrumbs.takeLast();
    QStringList dirCrumbs = pathCrumbs;
    NcDirNode* node = this->m_cachedTree.data();

    qInfo() << "fileName" << fileName;

    while (node && !pathCrumbs.isEmpty()) {
        qInfo() << "node name" << node->name;
        qInfo() << "crumb" << pathCrumbs[0];

        NcDirNode* nextNode = nullptr;
        for (NcDirNode* potentialNode : node->directories) {
            if (!potentialNode) {
                qDebug() << "potentialNode is a nullptr";
                continue;
            }

            qDebug() << potentialNode->name;
            if (potentialNode->name == pathCrumbs.first()) {
                qDebug() << "next node:" << potentialNode->name;
                nextNode = potentialNode;
                pathCrumbs.takeFirst();
                break;
            }
        }
        node = nextNode;
    }

    bool fileFound = false;
    if (node) {
        // examine the node's list of files for the file name
        for (QVariant file : node->files) {
            QVariantMap fileInfo = file.toMap();
            //qDebug() << "fileInfo" << fileInfo;
            if (fileInfo.value("name").toString() == fileName) {
                fileFound = true;
            }
        }
    }

    // Reliably find missing directories in the remote tree
    qInfo() << "Finding missing remote directory structure:" << this->m_remotePath << dirCrumbs;
    node = this->m_cachedTree.data();
    while (node && dirCrumbs.count() > 0) {
        for (NcDirNode* potentialNode : node->directories) {
            if (potentialNode->name == dirCrumbs.first()) {
                node = potentialNode;
                dirCrumbs.takeFirst();
                break;
            }
        }

        node = nullptr;
    }

    missingDirectories.append(dirCrumbs);

    return fileFound;
}

void NcSyncCommandUnit::expand(CommandEntity *previousCommandEntity)
{
    if (!this->m_client) {
        qWarning() << "Client is null, bailing out.";
        return;
    }

    if (!previousCommandEntity) {
        qWarning() << Q_FUNC_INFO << "entity is not valid.";
        return;
    }

    const QString commandType = previousCommandEntity->info().property(QStringLiteral("type")).toString();

    if (commandType != QStringLiteral("dirTree")) {
        qWarning() << Q_FUNC_INFO << "command isn't of type 'dirTree' but" << commandType;
        return;
    }

    NcDirTreeCommandUnit* treeCommandUnit = qobject_cast<NcDirTreeCommandUnit*>(previousCommandEntity);
    if (!treeCommandUnit) {
        qWarning() << "NcDirTreeCommandUnit couldn't be retrieved";
        return;
    }

    QVariantMap dirTreeCommandResult = treeCommandUnit->resultData();
    const bool success = dirTreeCommandResult.value(QStringLiteral("success")).toBool();
    if (!success) {
        if (!this->m_directoryCreation) {
            qInfo() << "trying to create target directory" << this->m_remotePath;
            this->m_directoryCreation = true;

            CommandEntity* mkdirCommand =
                    this->m_client->makeDirectoryRequest(this->m_remotePath, false);

            this->queue()->push_front(mkdirCommand);
            this->queue()->push_back(defaultCommandEntity(parent(),
                                                          this->m_client,
                                                          this->m_remotePath,
                                                          this->m_cachedTree));
            return;
        } else {
            qWarning() << "Failed to create remote target directory. Bailing out.";
            return;
        }
    }

    QVariant treeAsVariant = dirTreeCommandResult.value(QStringLiteral("tree"));
    this->m_cachedTree = treeAsVariant.value<QSharedPointer<NcDirNode> >();

    if (!this->m_cachedTree) {
        qWarning() << "NcDirNode couldn't be retrieved, bailing out.";
        return;
    }

    QDirIterator localIterator(this->m_localPath, QDirIterator::Subdirectories);

    // Avoid duplicate creation of the same directory as this will break the unit
    QStringList dirsToCreate;

    while (localIterator.hasNext()) {
        const QString sourcePath = localIterator.next();
        QFileInfo fileInfo(sourcePath);

        // Skip directories
        if (fileInfo.isDir())
            continue;

        QStringList missingDirectories;
        const bool exists = fileExistsRemotely(sourcePath, missingDirectories);
        qInfo() << "file" << sourcePath << "exists remotely?" << exists << ", missing directories:" << missingDirectories.join("/");

        /*const QString remoteRelativeFilePath = localFilePath.mid(this->m_localPath.length());
        const QString remoteRelativeDir = remoteRelativeFilePath.split('/', QString::SkipEmptyParts).join('/');
        NcDirNode* node = this->m_cachedTree->getNode(remoteRelativeDir);
        const bool uniqueIdExists = (node && node->containsFileWithUniqueId());*/

        // TODO: Remove files that don't exist remotely anymore
        // Might need a 3-way comparison (local past, local now, server now)
        // Policy: in case of doubt the server wins

        // Skip the file upload if it already exists remotely
        if (exists) {
            // Add the directory path to created dirs as to avoid breaking the unit
            QString missingRelativeDir = this->m_remotePath;
            for (QString remainingCrumb : missingDirectories) {
                missingRelativeDir += remainingCrumb + NODE_PATH_SEPARATOR;
            }
            if (!dirsToCreate.contains(missingRelativeDir))
                dirsToCreate.append(missingRelativeDir);
            continue;
        }

        // Create missing directories first if any
        if (missingDirectories.length() > 0) {
            QString missingRelativeDir = this->m_remotePath;
            for (QString remainingCrumb : missingDirectories) {
                missingRelativeDir += remainingCrumb + NODE_PATH_SEPARATOR;

                if (dirsToCreate.contains(missingRelativeDir))
                    continue;
                dirsToCreate.append(missingRelativeDir);

                qInfo() << "MkDirCommand:" << missingRelativeDir;
                CommandEntity* mkdirCommand =
                        this->m_client->makeDirectoryRequest(missingRelativeDir, false);
                this->queue()->push_front(mkdirCommand);
            }
        }

        // Upload the file
        qDebug() << sourcePath;
        qDebug() << this->m_localPath;

        // Build target directory string
        // Remove m_localPath from sourcePath and build list of directories
        QStringList targetDirectory =
                sourcePath.mid(this->m_localPath.length())
#if QT_VERSION < 0x060000
                .split('/', QString::SkipEmptyParts);
#else
                .split('/', Qt::SkipEmptyParts);
#endif

        QString relativeTargetDir;

        // Join directories back together without the file name
        if (!targetDirectory.isEmpty()) {
            targetDirectory.takeLast();
            relativeTargetDir = targetDirectory.join('/') + NODE_PATH_SEPARATOR;
        }

        qDebug() << "relativeTargetDir" << relativeTargetDir;

        const QString targetPath = this->m_remotePath + relativeTargetDir;
        CommandEntity* uploadCommand =
                this->m_client->fileUploadRequest(sourcePath, targetPath, fileInfo.lastModified(), false);

        this->queue()->push_back(uploadCommand);
    }

    qDebug() << "directories.length()" << this->m_cachedTree->directories.length();
}
