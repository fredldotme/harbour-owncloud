#include "mkdavdircommandentity.h"

MkDavDirCommandEntity::MkDavDirCommandEntity(QObject* parent,
                                             QString remotePath,
                                             QWebdav* client) :
    WebDavCommandEntity(parent, client)
{
    this->m_remotePath = remotePath;

    QMap<QString, QVariant> info;
    info["type"] = QStringLiteral("davMkDir");
    info["remotePath"] = remotePath;
    this->m_commandInfo = CommandEntityInfo(info);
}

bool MkDavDirCommandEntity::startWork()
{
    if (!this->m_client) {
        qWarning() << "No valid client object available, aborting";
        abortWork();
        return false;
    }
    this->m_reply = this->m_client->mkdir(this->m_remotePath);

    QObject::connect(m_reply,
#if QT_VERSION < 0x060000
                     static_cast<void(QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error),
#else
                     &QNetworkReply::errorOccurred,
#endif
                     this,
                     [=](QNetworkReply::NetworkError error) {
        qInfo() << "Remote directory creation" << this->m_remotePath << "failed,"
                << "error:" << error;
    });

    QObject::connect(m_reply, &QNetworkReply::finished, this, [=]() {
        qInfo() << "Remote directory creation" << this->m_remotePath << "complete.";
    });

    const bool canStart = WebDavCommandEntity::startWork();
    if (!canStart)
        return false;

    setState(RUNNING);
    return true;
}
