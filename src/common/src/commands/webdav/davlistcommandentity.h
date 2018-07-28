#ifndef DAVLISTCOMMANDENTITY_H
#define DAVLISTCOMMANDENTITY_H

#include <QObject>
#include "webdavcommandentity.h"

#include <qwebdavdirparser.h>

class DavListCommandEntity : public WebDavCommandEntity
{
    Q_OBJECT
public:
    explicit DavListCommandEntity(QObject* parent = Q_NULLPTR,
                                  QString remotePath = QStringLiteral(""),
                                  QWebdav* client = Q_NULLPTR,
                                  NextcloudSettingsBase* settings = Q_NULLPTR);

    bool startWork();

private:
    QWebdavDirParser m_parser;
    QString m_remotePath;
};

#endif // DAVLISTCOMMANDENTITY_H
