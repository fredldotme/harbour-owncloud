#ifndef FILEDOWNLOADCOMMANDENTITY_H
#define FILEDOWNLOADCOMMANDENTITY_H

#include <QObject>
#include <QFile>
#include <QDir>
#include <commandentity.h>
#include <settings/nextcloudsettingsbase.h>
#include <qwebdav.h>

class FileDownloadCommandEntity : public CommandEntity
{
    Q_OBJECT
public:
    explicit FileDownloadCommandEntity(QObject* parent = Q_NULLPTR,
                                       QString remotePath = QStringLiteral(""),
                                       QString localPath = QStringLiteral(""),
                                       NextcloudSettingsBase* settings = Q_NULLPTR,
                                       QWebdav* client = Q_NULLPTR);
    ~FileDownloadCommandEntity();
    bool isRunning() { return m_running; }

protected:
    void startWork();
    void abortWork();

    QWebdav* m_client = Q_NULLPTR;
    QNetworkReply* m_reply = Q_NULLPTR;
    QString m_remotePath = QStringLiteral("");
    QFile* m_localFile = Q_NULLPTR;
    QDir m_localDir;

private:
    bool m_running = false;
};

#endif // FILEDOWNLOADCOMMANDENTITY_H
