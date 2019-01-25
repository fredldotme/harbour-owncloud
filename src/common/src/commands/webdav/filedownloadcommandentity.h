#ifndef FILEDOWNLOADCOMMANDENTITY_H
#define FILEDOWNLOADCOMMANDENTITY_H

#include <QObject>
#include <QFile>
#include <QDir>
#include "webdavcommandentity.h"
#include <settings/nextcloudsettingsbase.h>

class FileDownloadCommandEntity : public WebDavCommandEntity
{
    Q_OBJECT
public:
    explicit FileDownloadCommandEntity(QObject* parent = Q_NULLPTR,
                                       QString remotePath = QStringLiteral(""),
                                       QString localPath = QStringLiteral(""),
                                       QWebdav* client = Q_NULLPTR);
    ~FileDownloadCommandEntity();

protected:
    bool startWork() Q_DECL_OVERRIDE;
    bool abortWork() Q_DECL_OVERRIDE;
    bool staticProgress() const Q_DECL_OVERRIDE { return false; }

    QString m_remotePath = QStringLiteral("");
    QFile* m_localFile = Q_NULLPTR;
    QDir m_localDir;

private:
    bool m_running = false;
};

#endif // FILEDOWNLOADCOMMANDENTITY_H
