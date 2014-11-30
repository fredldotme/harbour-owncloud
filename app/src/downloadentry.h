#ifndef DOWNLOADENTRY_H
#define DOWNLOADENTRY_H

#include <QObject>
#include <QNetworkReply>
#include <qwebdav.h>

#include "shellcommand.h"

class DownloadEntry : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ getName NOTIFY nameChanged)
    Q_PROPERTY(QString localPath READ getLocalPath NOTIFY localPathChanged)
    Q_PROPERTY(qint64 size READ getSize NOTIFY sizeChanged)
    Q_PROPERTY(qreal progress READ getProgress NOTIFY progressChanged)

public:
    DownloadEntry(QObject *parent = 0, QWebdav *webdav = 0,
                  QString name = "", QString remotePath = "",
                  QString localPath = "", qint64 size = 0, bool open = false);

    ~DownloadEntry();

    QString getName();
    QString getLocalPath();
    QString getRemotePath();
    qint64 getSize();
    qreal getProgress();

    void setProgress(qreal value);

    void startDownload();
    void pauseDownload();
    void cancelDownload();

private:
    QWebdav *webdav;

    QFile *localFile;

    bool m_open;

    QString m_name;
    QString m_localPath;
    QString m_remotePath;
    qint64 m_size;
    qreal m_progress;

signals:
    void nameChanged();
    void localPathChanged();
    void sizeChanged();
    void progressChanged();

    void downloadCompleted();

public slots:
    void handleReadComplete();

};

#endif // DOWNLOADENTRY_H
