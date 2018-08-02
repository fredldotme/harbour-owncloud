#include "ncdirtreecommandunit.h"

#include <commands/webdav/davlistcommandentity.h>

// NcDirTreeCommandUnit builds a tree view of the cloud instance

DavListCommandEntity* startingListCommand(QObject* parent,
                                          const QString& rootPath,
                                          QWebdav* client,
                                          NextcloudSettingsBase* settings)
{
    DavListCommandEntity* davListCommand =
            new DavListCommandEntity(parent, rootPath, true, client, settings);
    return davListCommand;
}

CommandEntityInfo defaultCommandInfo(const QString& rootPath)
{
    QVariantMap info;
    info.insert(QStringLiteral("type"), "dirTree");
    info.insert("rootPath", rootPath);
    return CommandEntityInfo(info);
}

NcDirTreeCommandUnit::NcDirTreeCommandUnit(QObject *parent,
                                           QWebdav *client,
                                           NextcloudSettingsBase *settings,
                                           QString rootPath) :
    CommandUnit(parent, {startingListCommand(parent, rootPath, client, settings)},
                defaultCommandInfo(rootPath)),
    m_client(client),
    m_settings(settings)
{
    if (!this->queue()->front()) {
        qCritical() << "The CommandUnit is supposed to have an entity in the queue but doesn't. Bailing out.";
        return;
    }

    // Start content list retrieval at the root node
    this->m_rootNode.name = this->queue()->front()->info().property(QStringLiteral("name")).toString();
    this->m_rootNode.parentNode = nullptr;
    this->m_currentNode = &this->m_rootNode;
}

void NcDirTreeCommandUnit::decideAdditionalWorkRequired(CommandEntity *entity)
{
    if (!entity)
        return;

    DavListCommandEntity* listCommand = qobject_cast<DavListCommandEntity*>(entity);
    if (!listCommand)
        return;

    // Command hasn't successfully finished
    if (!listCommand->isFinished())
        return;

    if (!this->m_currentNode) {
        qInfo() << "currentNode == nullptr, traversing remote tree should be complete.";
        return;
    }

    const QString remotePath = listCommand->info().property(QStringLiteral("remotePath")).toString();
    const QString directoryNameOfCommand = listCommand->info().property(QStringLiteral("remotePath")).toString();

    QVector<QVariant> files;
    QVector<NcDirNode> directories;

    const QVariantList directoryContent = listCommand->resultData().toList();

    for (const QVariant& tmpEntry : directoryContent) {
        // Skip invalid variants
        if (!tmpEntry.canConvert(QMetaType::QVariantMap))
            continue;

        QVariantMap entry = tmpEntry.toMap();
        const bool entryIsDirectory = entry.value(QStringLiteral("isDirectory")).toBool();
        const QString entryName = entry.value(QStringLiteral("name")).toString();

        qDebug() << "entryName:" << entryName;

        if (!entryIsDirectory) {
            // Add to the list of files and
            // continue with the next tmpEntry
            files.append(entry);
            continue;
        }

        // Prepare the necessary tree node
        // NcDirNode and the related DavListCommandEntity are inserted
        // into their respective lists/queues in the same order
        //
        NcDirNode node;
        node.name = entryName;
        node.parentNode = this->m_currentNode;

        directories.prepend(node);

        const QString fullPath = remotePath + entryName + "/";

        // then add required DavListCommandEntity
        DavListCommandEntity* additionalCommand =
                new DavListCommandEntity(parent(),
                                         fullPath,
                                         true,
                                         this->m_client,
                                         this->m_settings);
        this->queue()->push_front(additionalCommand);
        qDebug() << "DavListCommandEntity" << fullPath;
    }

    qDebug() << "directoryNameOfCommand" << directoryNameOfCommand;
    qDebug() << "current node" << this->m_currentNode->name;
    this->m_currentNode->files = files;
    this->m_currentNode->directories = directories;
    this->m_currentNode->directory_iterator = this->m_currentNode->directories.begin();

    // Iterate through child directories if possible
    if (this->m_currentNode->directory_iterator != this->m_currentNode->directories.end()) {
        // Essentially walking a level down
        this->m_currentNode = &(*this->m_currentNode->directory_iterator++);
    } else {
        // Iterated through all child nodes
        NcDirNode* parentNode = this->m_currentNode->parentNode;
        const bool hasParent = (parentNode != nullptr);
        const bool parentHasChildrenAvailable =
                (hasParent && parentNode->directory_iterator != parentNode->directories.end());

        qDebug() << parentHasChildrenAvailable;

        if (parentHasChildrenAvailable) {
            // Walk up a level and into the next child directory
            this->m_currentNode = &(*parentNode->directory_iterator++);
        } else {
            qDebug() << "parentIteratedAllChildren";
            // Walk up two levels and into the next child directory
            if (parentNode->parentNode)
                this->m_currentNode = &(*parentNode->parentNode->directory_iterator++);

            // At this point building the tree should be done
            // as all children of all nodes would be iterated.
            // Set the currentNode to null to avoid further traversion.
            this->m_currentNode = nullptr;
        }
    }
    qDebug() << Q_FUNC_INFO << "done";
}
