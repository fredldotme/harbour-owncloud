#include "ncsynccommandunit.h"

#include <commands/webdav/mkdavdircommandentity.h>
#include <commands/webdav/fileuploadcommandentity.h>
#include <commands/webdav/davproppatchcommandentity.h>

#include <QDir>
#include <QFile>
#include <QFileInfo>

CommandEntity* defaultCommandEntity(QObject* parent,
                                    QWebdav* client,
                                    QString remotePath,
                                    QSharedPointer<NcDirNode> cachedTree)
{
    return new NcDirTreeCommandUnit(parent, client, remotePath, cachedTree);
}

CommandEntityInfo defaultCommandInfo(const QString& localPath, const QString& remotePath)
{
    QVariantMap info;
    info.insert(QStringLiteral("type"), "dirTree");
    info.insert("localPath", localPath);
    info.insert("remotePath", remotePath);
    return CommandEntityInfo(info);
}

NcSyncCommandUnit::NcSyncCommandUnit(QObject* parent,
                                     QWebdav* client,
                                     QString localPath,
                                     QString remotePath,
                                     QSharedPointer<NcDirNode> cachedTree) :
    CommandUnit(parent, {defaultCommandEntity(parent, client, remotePath, cachedTree)},
                defaultCommandInfo(localPath, remotePath)),
    m_localPath(localPath),
    m_remotePath(remotePath),
    m_client(client),
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
    QStringList pathCrumbs = relativeFilePath.split("/", QString::SkipEmptyParts);
    const QString fileName = pathCrumbs.takeLast();
    NcDirNode* node = this->m_cachedTree.data();

    qDebug() << "fileName" << fileName;
    while (node && !pathCrumbs.isEmpty()) {
        qDebug() << "node name" << node->name;
        qDebug() << "crumb" << pathCrumbs[0];

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

    const bool isFile = (pathCrumbs.isEmpty());
    bool fileFound = false;
    if (isFile && node) {
        // examine the node's list of files for the file name
        for (QVariant file : node->files) {
            QVariantMap fileInfo = file.toMap();
            //qDebug() << "fileInfo" << fileInfo;
            if (fileInfo.value("name").toString() == fileName) {
                fileFound = true;
            }
        }
    }

    missingDirectories.append(pathCrumbs);
    return fileFound;
}

void NcSyncCommandUnit::expand(CommandEntity *previousCommandEntity)
{
    if (!previousCommandEntity) {
        qWarning() << Q_FUNC_INFO << "entity is not valid.";
        return;
    }

    const QString commandType = previousCommandEntity->info().property(QStringLiteral("type")).toString();

    if (commandType != QStringLiteral("dirTree")) {
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
            qInfo() << "trying to create target directory";
            this->m_directoryCreation = true;

            MkDavDirCommandEntity* mkdirCommand =
                    new MkDavDirCommandEntity(parent(),
                                              this->m_remotePath,
                                              this->m_client);

            this->queue()->push_back(mkdirCommand);
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
        qDebug() << "file exists remotely?" << exists;

        /*const QString remoteRelativeFilePath = localFilePath.mid(this->m_localPath.length());
        const QString remoteRelativeDir = remoteRelativeFilePath.split('/', QString::SkipEmptyParts).join('/');
        NcDirNode* node = this->m_cachedTree->getNode(remoteRelativeDir);
        const bool uniqueIdExists = (node && node->containsFileWithUniqueId());*/

        // Skip the file upload if it already exists remotely
        if (exists)
            continue;

        // Create missing directories first if any
        if (missingDirectories.length() > 0) {
            QString missingRelativeDir = this->m_remotePath;
            for (QString remainingCrumb : missingDirectories) {
                missingRelativeDir += remainingCrumb + NODE_PATH_SEPARATOR;

                if (dirsToCreate.contains(missingRelativeDir))
                    break;
                dirsToCreate.append(missingRelativeDir);

                MkDavDirCommandEntity* mkdirCommand =
                        new MkDavDirCommandEntity(parent(),
                                                  missingRelativeDir,
                                                  this->m_client);
                this->queue()->push_back(mkdirCommand);
            }
        }

        // Upload the file
        qDebug() << sourcePath;
        qDebug() << this->m_localPath;

        // Build target directory string
        // Remove m_localPath from sourcePath and build list of directories
        QStringList targetDirectory =
                sourcePath.mid(this->m_localPath.length())
                .split('/', QString::SkipEmptyParts);

        QString relativeTargetDir;

        // Join directories back together without the file name
        if (!targetDirectory.isEmpty()) {
            targetDirectory.takeLast();
            relativeTargetDir = targetDirectory.join('/') + NODE_PATH_SEPARATOR;
        }

        qDebug() << "relativeTargetDir" << relativeTargetDir;

        const QString targetPath = this->m_remotePath + relativeTargetDir;
        FileUploadCommandEntity* uploadCommand = Q_NULLPTR;
        DavPropPatchCommandEntity* propPatchCommand  = Q_NULLPTR;

        {
            uploadCommand = new FileUploadCommandEntity(parent(),
                                                        sourcePath,
                                                        targetPath,
                                                        this->m_client);
        }
        {
            QWebdav::PropValues props;
            QMap<QString, QVariant> propMap;

            // Last modified in seconds
            propMap["lastmodified"] = (QVariant)(fileInfo.lastModified().toMSecsSinceEpoch() / 1000);
            props["DAV:"] = propMap;


            propPatchCommand =
                    new DavPropPatchCommandEntity(this, targetPath + fileInfo.fileName(),
                                                  props, this->m_client);
        }

        this->queue()->push_back(new CommandUnit(parent(),
        {uploadCommand, propPatchCommand}, uploadCommand->info()));
    }

    qDebug() << "directories.length()" << this->m_cachedTree->directories.length();
}
