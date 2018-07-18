#ifndef REMOTEDIRECTORYCOMMANDENTITY_H
#define REMOTEDIRECTORYCOMMANDENTITY_H

#include <QObject>
#include <commandentity.h>
#include <settings/nextcloudsettingsbase.h>
#include <qwebdav.h>

class RemoteDirectoryCommandEntity : public CommandEntity
{
    Q_OBJECT
public:
    explicit RemoteDirectoryCommandEntity(QObject* parent = Q_NULLPTR,
                                          QString remotePath = QStringLiteral(""),
                                          NextcloudSettingsBase* settings = Q_NULLPTR);
    ~RemoteDirectoryCommandEntity();

    bool isRunning() { return m_running; }

private:
    void startWork();
    void abortWork();

    bool m_running = false;
    QString m_remotePath = QStringLiteral("");
    QWebdav* m_client = Q_NULLPTR;
    QNetworkReply* m_reply = Q_NULLPTR;
};

#endif // REMOTEDIRECTORYCOMMANDENTITY_H
