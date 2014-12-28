#ifndef UPLOADENTRY_H
#define UPLOADENTRY_H

#include <QObject>
#include <QStringList>
#include <QNetworkReply>

class QWebdav;

class UploadEntry : public QObject
{
    Q_OBJECT
public:
    explicit UploadEntry(QString localPath, QString remotePath, QStringList pathsToCreate, QWebdav *connection, QObject *parent = 0);
    ~UploadEntry();

    QString localPath() { return m_localPath; }
    bool succeeded() { return m_succeeded; }

signals:
    void uploadFailed(QString errorMessage);
    void finished();

private slots:
    void doUpload();
    void errorHandler(QNetworkReply::NetworkError);

private:
    void createDirectory();

    QWebdav *m_connection;
    QStringList m_pathsToCreate;
    QString m_localPath;
    QString m_remotePath;
    bool m_succeeded;
};

#endif // UPLOADENTRY_H
