#ifndef NCDIRTREECOMMANDUNIT_H
#define NCDIRTREECOMMANDUNIT_H

#include <QObject>
#include <QVector>
#include <QSharedPointer>
#include <commandunit.h>
#include <settings/nextcloudsettingsbase.h>
#include <provider/storage/cloudstorageprovider.h>

const QString NODE_PATH_SEPARATOR = QStringLiteral("/");

class NcDirNode : public QObject
{
    Q_OBJECT

public:
    ~NcDirNode()
    {
        while (!directories.isEmpty()) {
            NcDirNode* backReference = this->directories.back();
            this->directories.pop_back();
            if (backReference) {
                delete backReference;
            }
        }
    }

    bool containsDirWithUniqueId(const QString& uniqueId)
    {
        if (uniqueId.isEmpty())
            return false;

        for (const NcDirNode* directory : this->directories) {
            if (directory->uniqueId == uniqueId)
                return true;
        }
        return false;
    }

    bool containsFileWithUniqueId(const QString& uniqueId)
    {
        if (uniqueId.isEmpty())
            return false;

        for (const QVariantMap& file : this->files) {
            const QString fileUniqueId = file.value(QStringLiteral("uniqueId")).toString();
            if (fileUniqueId.isEmpty())
                continue;
            if (uniqueId == fileUniqueId)
                return true;
        }
        return false;
    }

    NcDirNode* getNode(const QString& path)
    {
        if (this->directories.empty())
            return nullptr;

#if QT_VERSION < 0x060000
        QStringList crumbs = path.split("/", QString::SkipEmptyParts);
#else
        QStringList crumbs = path.split("/", Qt::SkipEmptyParts);
#endif
        if (crumbs.empty())
            return nullptr;

        QVector<NcDirNode*>::iterator potentialNodeIterator = directories.begin();
        NcDirNode* potentialNode = this;
        while (potentialNode) {
            if (potentialNode->name == crumbs.first()) {
                crumbs.takeFirst();
            }

            if (crumbs.empty())
                break;

            if (potentialNodeIterator == directories.end()) {
                potentialNode = nullptr;
                break;
            }

            potentialNode = *(potentialNodeIterator++);
        }

        // If there are remaining crumbs the path couldn't be fully resolved
        if (!crumbs.empty())
            return nullptr;

        return potentialNode;
    }

    NcDirNode* parentNode = nullptr;

    QString name;
    QString uniqueId;
    QVector<QVariantMap> files;
    QVector<NcDirNode*> directories;
    QVector<NcDirNode*>::iterator directory_iterator;
};
Q_DECLARE_METATYPE(NcDirNode*)

class NcDirTreeCommandUnit : public CommandUnit
{
    Q_OBJECT

public:
    NcDirTreeCommandUnit(QObject* parent = Q_NULLPTR,
                         CloudStorageProvider* client = Q_NULLPTR,
                         QString rootPath = NODE_PATH_SEPARATOR,
                         QSharedPointer<NcDirNode> cachedTree = QSharedPointer<NcDirNode>(),
                         unsigned int depth = UINT_MAX);

protected:
    void expand(CommandEntity* previousCommandEntity) Q_DECL_OVERRIDE;

private:
    CloudStorageProvider* m_client;
    AccountBase* m_settings;
    QSharedPointer<NcDirNode> m_rootNode;

    // As the list commands are run serially we can keep
    // a pointer to the currently processed node to avoid
    // additional traversion of the whole tree.
    NcDirNode* m_currentNode = Q_NULLPTR;
    unsigned int m_depth;
    unsigned int m_currentDepthLevel = 0;
};

#endif // NCDIRTREECOMMANDUNIT_H
