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
                                     QWebdav* client = Q_NULLPTR);

protected:
    bool startWork() Q_DECL_OVERRIDE;
    bool staticProgress() const Q_DECL_OVERRIDE { return false; }

private:
    bool m_running = false;
    QFile* m_localFile = Q_NULLPTR;
    QString m_remotePath;
};

#endif // FILEUPLOADCOMMANDENTITY_H
