#include "ncdirtreecommandunit.h"

#include <commands/webdav/davlistcommandentity.h>

// NcDirTreeCommandUnit builds a tree view of the cloud instance

#define REACHED_END(node) (node->directory_iterator == node->directories.end())

DavListCommandEntity* startingListCommand(QObject* parent,
                                          const QString& rootPath,
                                          QWebdav* client)
{
    DavListCommandEntity* davListCommand =
            new DavListCommandEntity(parent, rootPath, true, client);
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
                                           QString rootPath) :
    CommandUnit(parent, {startingListCommand(parent, rootPath, client)},
                defaultCommandInfo(rootPath)),
    m_client(client)
{
    if (!this->queue()->front()) {
        qCritical() << "The CommandUnit is supposed to have an entity in the queue but doesn't. Bailing out.";
        return;
    }

    // Start content list retrieval at the root node
    this->m_rootNode = new NcDirNode;
    this->m_rootNode->name = this->queue()->front()->info().property(QStringLiteral("name")).toString();
    this->m_rootNode->parentNode = nullptr;
    this->m_currentNode = this->m_rootNode;
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
    QVector<NcDirNode*> directories;

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
        NcDirNode* node = new NcDirNode;
        node->name = entryName;
        node->parentNode = this->m_currentNode;

        directories.prepend(node);

        const QString fullPath = remotePath + entryName + "/";

        // then add required DavListCommandEntity
        DavListCommandEntity* additionalCommand =
                new DavListCommandEntity(parent(),
                                         fullPath,
                                         true,
                                         this->m_client);
        this->queue()->push_front(additionalCommand);
        qDebug() << "DavListCommandEntity" << fullPath;
    }

    qDebug() << "directoryNameOfCommand" << directoryNameOfCommand;
    qDebug() << "current node" << this->m_currentNode->name;
    this->m_currentNode->files = files;
    this->m_currentNode->directories = directories;
    this->m_currentNode->directory_iterator = this->m_currentNode->directories.begin();

    NcDirNode* potentialNode = this->m_currentNode;
    while (potentialNode) {
        if (REACHED_END(potentialNode)) {
            potentialNode = potentialNode->parentNode;
        } else {
            this->m_currentNode = *potentialNode->directory_iterator++;
            break;
        }
    }

    if (!this->m_currentNode) {
        // At this point building the tree should be done
        // as all children of all nodes would be iterated.
        // Point resultData to the root node as finalization step.
        this->m_resultData = QVariant::fromValue<NcDirNode*>(this->m_rootNode);
    }

    qDebug() << Q_FUNC_INFO << "done";
}
