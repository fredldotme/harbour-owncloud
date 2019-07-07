#include "ncdirtreecommandunit.h"
#include <QSharedPointer>

#include <commands/webdav/davlistcommandentity.h>

// NcDirTreeCommandUnit builds a tree view of the cloud instance

#define REACHED_END(node) (node->directory_iterator == node->directories.end())

inline QVariantMap buildResultData(bool success, QSharedPointer<NcDirNode> tree)
{
    QVariantMap result;
    result.insert(QStringLiteral("success"),
                  success);
    result.insert(QStringLiteral("tree"),
                  QVariant::fromValue(tree));
    return result;
}

CommandEntity* startingListCommand(const QString& rootPath,
                                   CloudStorageProvider* client)
{
    if (!client) {
        qWarning() << Q_FUNC_INFO << "client is null";
        return nullptr;
    }
    CommandEntity* davListCommand =
            client->directoryListingRequest(rootPath, false, false);
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
                                           CloudStorageProvider *client,
                                           QString rootPath,
                                           QSharedPointer<NcDirNode> cachedTree) :
    CommandUnit(parent, {startingListCommand(rootPath, client)},
                defaultCommandInfo(rootPath)),
    m_client(client)
{
    if (!this->queue()->front()) {
        qCritical() << "The CommandUnit is supposed to have an entity in the queue but doesn't. Bailing out.";
        return;
    }

    if (!cachedTree.isNull()) {
        this->m_rootNode = cachedTree;
    } else {
        this->m_rootNode = QSharedPointer<NcDirNode>(new NcDirNode);
        this->m_rootNode->name = this->queue()->front()->info().property(QStringLiteral("name")).toString();
        this->m_rootNode->parentNode = nullptr;
    }

    // Start content list retrieval at the root node
    this->m_currentNode = this->m_rootNode.data();
}

void NcDirTreeCommandUnit::expand(CommandEntity* previousCommandEntity)
{
    if (!previousCommandEntity)
        return;

    // Command hasn't successfully finished
    if (!previousCommandEntity->isFinished()) {
        // iterating through child directories wouldn't be possible,
        // so go back to the parent node if possible.
        if (this->m_currentNode)
            this->m_currentNode = this->m_currentNode->parentNode;
    }

    if (!this->m_currentNode) {
        qInfo() << "currentNode == nullptr, traversing remote tree should be complete.";
        return;
    }

    const QVariantMap commandResult = previousCommandEntity->resultData();
    const int httpCode = commandResult.value(QStringLiteral("httpCode")).toInt();
    const bool success = (httpCode >= 200 && httpCode < 300);

    if (!success) {
        qInfo() << "Parsing directory list failed, ignoring";
        if (this->m_currentNode->name == NODE_PATH_SEPARATOR)
            this->m_resultData = buildResultData(success, this->m_rootNode);
        return;
    }

    const QString remotePath = previousCommandEntity->info().property(QStringLiteral("remotePath")).toString();
    const QVariantList directoryContent = commandResult.value(QStringLiteral("dirContent")).toList();

    for (const QVariant& tmpEntry : directoryContent) {
        // Skip invalid variants
        if (!tmpEntry.canConvert(QMetaType::QVariantMap))
            continue;

        const QVariantMap entry = tmpEntry.toMap();
        const bool entryIsDirectory = entry.value(QStringLiteral("isDirectory")).toBool();
        const QString entryName = entry.value(QStringLiteral("name")).toString();
        const QString uniqueId = entry.value(QStringLiteral("uniqueId")).toString();

        qDebug() << "entryName:" << entryName;

        // Skip directory examination/file appendage in case the unique ID (entity tag) already exists
        const bool uniqueIdsMatch = entryIsDirectory ?
                    this->m_currentNode->containsDirWithUniqueId(uniqueId) :
                    this->m_currentNode->containsFileWithUniqueId(uniqueId);

        if (uniqueIdsMatch)
            continue;

        if (!entryIsDirectory) {
            // Add to the list of files and
            // continue with the next tmpEntry
            this->m_currentNode->files.append(entry);
            continue;
        }

        // Prepare the necessary tree node
        // NcDirNode and the related DavListCommandEntity are inserted
        // into their respective lists/queues in the same order
        NcDirNode* node = new NcDirNode;
        node->name = entryName;
        node->uniqueId = uniqueId ;
        node->parentNode = this->m_currentNode;
        this->m_currentNode->directories.prepend(node);

        const QString fullPath = remotePath + entryName + NODE_PATH_SEPARATOR;

        // then add required DavListCommandEntity
        CommandEntity* additionalCommand = this->m_client->directoryListingRequest(fullPath, false, false);
        this->queue()->push_front(additionalCommand);
        qDebug() << "DavListCommandEntity" << fullPath;
    }

    qDebug() << "remotePath of command" << remotePath;
    qDebug() << "current node" << this->m_currentNode->name;
    this->m_currentNode->directory_iterator = this->m_currentNode->directories.begin();

    NcDirNode* potentialNode = this->m_currentNode;
    while (potentialNode) {
        if (REACHED_END(potentialNode)) {
            potentialNode->directory_iterator = potentialNode->directories.begin();
            potentialNode = potentialNode->parentNode;
        } else {
            this->m_currentNode = *potentialNode->directory_iterator++;
            break;
        }
    }

    if (!potentialNode) {
        // At this point building the tree should be done
        // as all children of all nodes would be iterated.
        // Point resultData to the root node as finalization step.
        qDebug() << "Setting result" << this->m_rootNode;
        this->m_resultData = buildResultData(success, this->m_rootNode);
        this->m_currentNode = Q_NULLPTR;
    }

    qDebug() << Q_FUNC_INFO << "done";
}
