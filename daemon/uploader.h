#ifndef UPLOADER_H
#define UPLOADER_H

#include "qwebdav.h"
#include "qwebdavdirparser.h"

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

public slots:
    void fileFound(QString filePath);

private slots:
    void settingsChanged();
    void uploadFinished();

    void remoteListingFinished();

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
};


#endif//UPLOADER_H
