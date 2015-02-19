#ifndef UPLOADER_H
#define UPLOADER_H

#include "qwebdav.h"
#include "qwebdavdirparser.h"
#include "networkmonitor.h"
#include "uploadentry.h"

#include <QObject>
#include <QSet>

class Uploader : public QObject
{
    Q_OBJECT

public:
    Uploader(QObject *parent = 0);

private:
    void abort();

signals:
    void uploadError(QString errorMessage);
    void localPathUpdated();
    void connectError(QString errorMessage);
    void fileUploaded(QString file);
    void uploadingChanged(bool uploading);

public slots:
    void fileFound(QString filePath);
    void setOnline(bool online);
    void settingsChanged();

private slots:
    void uploadFinished();
    void remoteListingFinished();
    void resetReply();

private:
    void uploadFile();
    void getExistingRemote();
    void remotePathCreated();
    void applySettings();

    QString relativeToRemote(QString path);
    QString relativeLocalPath(QString absolutePath);
    QString relativeRemotePath(QString absolutePath);

    QSet<QString> m_existingFiles;
    QSet<QString> m_existingDirs;

    QNetworkReply *m_currentReply;
    UploadEntry *m_currentEntry;
    QList<QString> m_uploadQueue;
    QWebdav m_connection;
    QWebdavDirParser m_remoteDir;
    bool m_uploading;
    bool m_fetchedExisting;
    QString m_remotePath;
    QStringList m_dirsToFetch;
    bool m_online;
};


#endif//UPLOADER_H
