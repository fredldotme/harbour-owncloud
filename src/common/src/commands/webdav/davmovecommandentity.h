#ifndef DAVMOVECOMMANDENTITY_H
#define DAVMOVECOMMANDENTITY_H

#include <QObject>
#include "webdavcommandentity.h"

class DavMoveCommandEntity : public WebDavCommandEntity
{
    Q_OBJECT
public:
    explicit DavMoveCommandEntity(QObject* parent = Q_NULLPTR,
                                  QString fromPath = QStringLiteral(""),
                                  QString toPath = QStringLiteral(""),
                                  QWebdav* client = Q_NULLPTR);

private:
    bool startWork();

    QString m_fromPath = QStringLiteral("");
    QString m_toPath = QStringLiteral("");

};

#endif // DAVMOVECOMMANDENTITY_H
