#ifndef NCSYNCCOMMANDUNIT_H
#define NCSYNCCOMMANDUNIT_H

#include <QObject>
#include <commandunit.h>
#include <qwebdav.h>
#include <settings/nextcloudsettingsbase.h>
#include <commands/sync/ncdirtreecommandunit.h>
#include <QSharedPointer>

class NcSyncCommandUnit : public CommandUnit
{
    Q_OBJECT
public:
    explicit NcSyncCommandUnit(QObject* parent = Q_NULLPTR,
                               QWebdav* client = Q_NULLPTR,
                               QString localPath = QStringLiteral(""),
                               QString remotePath = QStringLiteral(""),
                               QSharedPointer<NcDirNode> cachedTree = QSharedPointer<NcDirNode>(Q_NULLPTR));

    QSharedPointer<NcDirNode> cachedTree();

protected:
    void expand(CommandEntity* previousCommandEntity);

private:
    bool fileExistsRemotely(const QString& localFilePath,
                            QStringList& missingDirectories);

    QString m_localPath;
    QString m_remotePath;
    QWebdav* m_client = Q_NULLPTR;
    QSharedPointer<NcDirNode> m_cachedTree;
    bool m_directoryCreation = false;

};

#endif // NCSYNCCOMMANDUNIT_H
