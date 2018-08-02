#ifndef MKDAVDIRCOMMANDENTITY_H
#define MKDAVDIRCOMMANDENTITY_H

#include <QObject>
#include "webdavcommandentity.h"
#include <settings/nextcloudsettingsbase.h>

class MkDavDirCommandEntity : public WebDavCommandEntity
{
    Q_OBJECT
public:
    explicit MkDavDirCommandEntity(QObject* parent = Q_NULLPTR,
                                   QString remotePath = QStringLiteral(""),
                                   QWebdav* client = Q_NULLPTR);

private:
    bool startWork();

    QString m_remotePath = QStringLiteral("");
};

#endif // MKDAVDIRCOMMANDENTITY_H
