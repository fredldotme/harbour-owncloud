#ifndef FILEDOWNLOADCOMMANDENTITY_H
#define FILEDOWNLOADCOMMANDENTITY_H

#include <QObject>
#include <QFile>
#include <QDir>
#include <commands/webdavcommandentity.h>
#include <settings/nextcloudsettingsbase.h>
#include <qwebdav.h>

class FileDownloadCommandEntity : public WebDavCommandEntity
{
    Q_OBJECT
public:
    explicit FileDownloadCommandEntity(QObject* parent = Q_NULLPTR,
                                       QString remotePath = QStringLiteral(""),
                                       QString localPath = QStringLiteral(""),
                                       QWebdav* client = Q_NULLPTR,
                                       NextcloudSettingsBase* settings = Q_NULLPTR);

protected:
    void startWork();

    QString m_remotePath = QStringLiteral("");
    QFile* m_localFile = Q_NULLPTR;
    QDir m_localDir;

private:
    bool m_running = false;
};

#endif // FILEDOWNLOADCOMMANDENTITY_H
