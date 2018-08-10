#ifndef NCDIRTREECOMMANDUNIT_H
#define NCDIRTREECOMMANDUNIT_H

#include <QObject>
#include <commandunit.h>
#include <qwebdav.h>
#include <settings/nextcloudsettingsbase.h>

class NcDirNode : public QObject
{
    Q_OBJECT

public:
    ~NcDirNode()
    {
        qDebug() << Q_FUNC_INFO;
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
        for (const NcDirNode* directory : directories) {
            if (directory->uniqueId == uniqueId)
                return true;
        }
        return false;
    }

    NcDirNode* parentNode = nullptr;

    QString name;
    QString uniqueId;
    QVector<QVariant> files;
    QVector<NcDirNode*> directories;
    QVector<NcDirNode*>::iterator directory_iterator;
};
Q_DECLARE_METATYPE(NcDirNode*)

class NcDirTreeCommandUnit : public CommandUnit
{
    Q_OBJECT

public:
    NcDirTreeCommandUnit(QObject* parent = Q_NULLPTR,
                         QWebdav* client = Q_NULLPTR,
                         QString rootPath = QStringLiteral("/"),
                         QSharedPointer<NcDirNode> cachedTree = QSharedPointer<NcDirNode>());

protected:
    void decideAdditionalWorkRequired(CommandEntity *entity) Q_DECL_OVERRIDE;

private:
    QWebdav* m_client;
    NextcloudSettingsBase* m_settings;
    QSharedPointer<NcDirNode> m_rootNode;

    // As the list commands are run serially we can keep
    // a pointer to the currently processed node to avoid
    // additional traversion of the whole tree.
    NcDirNode* m_currentNode = Q_NULLPTR;
};

#endif // NCDIRTREECOMMANDUNIT_H
