#ifndef FILEUPLOADCOMMANDENTITY_H
#define FILEUPLOADCOMMANDENTITY_H

#include <QObject>
#include <QFile>
#include <commandentity.h>
#include <settings/nextcloudsettingsbase.h>
#include <qwebdav.h>

class FileUploadCommandEntity : public CommandEntity
{
    Q_OBJECT
public:
    explicit FileUploadCommandEntity(QObject* parent = Q_NULLPTR,
                                     QString localPath = QStringLiteral(""),
                                     QString remotePath = QStringLiteral(""),
                                     NextcloudSettingsBase* settings = Q_NULLPTR);
    ~FileUploadCommandEntity();

    bool isRunning() { return this->m_running; }

protected:
    void startWork();
    void abortWork();

private:
    void setModifiedTime();

    bool m_running = false;
    QWebdav* m_client = Q_NULLPTR;
    QNetworkReply* m_reply = Q_NULLPTR;
    QFile* m_localFile = Q_NULLPTR;
    QString m_remotePath;

signals:

public slots:
};

#endif // FILEUPLOADCOMMANDENTITY_H
