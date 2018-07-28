#include "davmovecommandentity.h"

DavMoveCommandEntity::DavMoveCommandEntity(QObject* parent,
                                           QString fromPath,
                                           QString toPath,
                                           QWebdav* client,
                                           NextcloudSettingsBase* settings) :
    WebDavCommandEntity(parent, client, settings)
{
    this->m_fromPath = fromPath;
    this->m_toPath = toPath;

    QMap<QString, QVariant> info;
    info["type"] = QStringLiteral("davMove");
    info["fromPath"] = fromPath;
    info["toPath"] = toPath;
    this->m_commandInfo = CommandEntityInfo(info);
}

bool DavMoveCommandEntity::startWork()
{
    this->m_reply = this->m_client->move(this->m_fromPath, this->m_toPath);

    QObject::connect(m_reply, &QNetworkReply::finished, this, [=]() {
        qInfo() << "Dav entity moving" << this->m_fromPath << "to" << this->m_toPath << "complete.";
    });

    const bool canStart = WebDavCommandEntity::startWork();
    if (!canStart)
        return false;

    setState(RUNNING);
    return true;
}
