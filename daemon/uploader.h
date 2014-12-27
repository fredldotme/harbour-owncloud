#ifndef UPLOADER_H
#define UPLOADER_H

#include "qwebdav.h"
#include "qwebdavdirparser.h"
#include "networkmonitor.h"

#include <QObject>
#include <QSet>

class Uploader : public QObject
{
    Q_OBJECT

public:
    Uploader(QObject *parent = 0);

signals:
    void uploadError(QString errorMessage);
    void localPathUpdated();
    void connectError(QString errorMessage);
    void fileUploaded(QString file);

public slots:
    void fileFound(QString filePath);
    void setSuspended(bool suspended);

private slots:
    void settingsChanged();
    void uploadFinished();
    void remoteListingFinished();
    void onlineChanged(bool online);

private:
    void uploadFile();
    void getExistingRemote();
    void remotePathCreated();
    QString relativeToRemote(QString path);

    QString relativeLocalPath(QString absolutePath);
    QString relativeRemotePath(QString absolutePath);

    QSet<QString> m_existingFiles;
    QSet<QString> m_existingDirs;

    QList<QString> m_uploadQueue;
    QWebdav m_connection;
    QWebdavDirParser m_remoteDir;
    bool m_uploading;
    bool m_fetchedExisting;
    QString m_remotePath;
    QStringList m_dirsToFetch;
    bool m_suspended;
};


#endif//UPLOADER_H
