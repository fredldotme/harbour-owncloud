#ifndef MKDAVDIRCOMMANDENTITY_H
#define MKDAVDIRCOMMANDENTITY_H

#include <QObject>
#include <commands/webdavcommandentity.h>
#include <settings/nextcloudsettingsbase.h>
#include <qwebdav.h>

class MkDavDirCommandEntity : public WebDavCommandEntity
{
    Q_OBJECT
public:
    explicit MkDavDirCommandEntity(QObject* parent = Q_NULLPTR,
                                   QString remotePath = QStringLiteral(""),
                                   QWebdav* client = Q_NULLPTR,
                                   NextcloudSettingsBase* settings = Q_NULLPTR);

private:
    void startWork();

    QString m_remotePath = QStringLiteral("");
};

#endif // MKDAVDIRCOMMANDENTITY_H
