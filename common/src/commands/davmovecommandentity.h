#ifndef DAVMOVECOMMANDENTITY_H
#define DAVMOVECOMMANDENTITY_H

#include <QObject>
#include <commands/webdavcommandentity.h>

class DavMoveCommandEntity : public WebDavCommandEntity
{
    Q_OBJECT
public:
    explicit DavMoveCommandEntity(QObject* parent = Q_NULLPTR,
                                  QString fromPath = QStringLiteral(""),
                                  QString toPath = QStringLiteral(""),
                                  QWebdav* client = Q_NULLPTR,
                                  NextcloudSettingsBase* settings = Q_NULLPTR);

private:
    void startWork();

    QString m_fromPath = QStringLiteral("");
    QString m_toPath = QStringLiteral("");

};

#endif // DAVMOVECOMMANDENTITY_H