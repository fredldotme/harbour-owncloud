#ifndef DAVRMCOMMANDENTITY_H
#define DAVRMCOMMANDENTITY_H

#include <QObject>
#include "webdavcommandentity.h"

class DavRmCommandEntity : public WebDavCommandEntity
{
    Q_OBJECT
public:
    explicit DavRmCommandEntity(QObject* parent = Q_NULLPTR,
                                QString remotePath = QStringLiteral(""),
                                QWebdav* client = Q_NULLPTR,
                                NextcloudSettingsBase* settings = Q_NULLPTR);

private:
    bool startWork();

    QString m_remotePath = QStringLiteral("");
};

#endif // DAVRMCOMMANDENTITY_H
