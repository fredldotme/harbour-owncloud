#ifndef DAVPROPPATCHCOMMANDENTITY_H
#define DAVPROPPATCHCOMMANDENTITY_H

#include <QObject>
#include "webdavcommandentity.h"

class DavPropPatchCommandEntity : public WebDavCommandEntity
{
    Q_OBJECT
public:
    explicit DavPropPatchCommandEntity(QObject* parent = Q_NULLPTR,
                                       QString remotePath = QStringLiteral(""),
                                       QWebdav::PropValues m_props = QWebdav::PropValues(),
                                       QWebdav* client = Q_NULLPTR);

private:
    bool startWork();

    QString m_remotePath;
    QWebdav::PropValues m_props;
};

#endif // DAVPROPPATCHCOMMANDENTITY_H
