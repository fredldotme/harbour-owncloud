#ifndef NCDIRTREECOMMANDUNIT_H
#define NCDIRTREECOMMANDUNIT_H

#include <QObject>
#include <commandunit.h>
#include <qwebdav.h>
#include <settings/nextcloudsettingsbase.h>

class NcDirNode {
    Q_GADGET

public:
    NcDirNode* parentNode = nullptr;

    QString name;
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
                         NextcloudSettingsBase* settings = Q_NULLPTR,
                         QString rootPath = QStringLiteral("/"));

protected:
    void decideAdditionalWorkRequired(CommandEntity *entity) Q_DECL_OVERRIDE;

private:
    QWebdav* m_client;
    NextcloudSettingsBase* m_settings;
    NcDirNode* m_rootNode = Q_NULLPTR;

    // As the list commands are run serially we can keep
    // a pointer to the currently processed node to avoid
    // additional traversion of the whole tree.
    NcDirNode* m_currentNode = Q_NULLPTR;
};

#endif // NCDIRTREECOMMANDUNIT_H
