#ifndef FILEUPLOADCOMMANDENTITY_H
#define FILEUPLOADCOMMANDENTITY_H

#include <QObject>
#include <QFile>
#include "webdavcommandentity.h"
#include <settings/nextcloudsettingsbase.h>

class FileUploadCommandEntity : public WebDavCommandEntity
{
    Q_OBJECT

public:
    explicit FileUploadCommandEntity(QObject* parent = Q_NULLPTR,
                                     QString localPath = QStringLiteral(""),
                                     QString remotePath = QStringLiteral(""),
                                     QWebdav* client = Q_NULLPTR,
                                     NextcloudSettingsBase* settings = Q_NULLPTR);

protected:
    bool startWork();

private:
    void setModifiedTime();

    bool m_running = false;
    QFile* m_localFile = Q_NULLPTR;
    QString m_remotePath;

signals:

public slots:
};

#endif // FILEUPLOADCOMMANDENTITY_H
