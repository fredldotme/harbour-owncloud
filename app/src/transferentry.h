#ifndef TRANSFERENTRY_H
#define TRANSFERENTRY_H

#include <QObject>
#include <QNetworkReply>
#include <qwebdav.h>

#include "shellcommand.h"

class TransferEntry : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ getName NOTIFY nameChanged)
    Q_PROPERTY(QString localPath READ getLocalPath NOTIFY localPathChanged)
    Q_PROPERTY(qint64 size READ getSize NOTIFY sizeChanged)
    Q_PROPERTY(int direction READ getTransferDirection)

public:
    enum TransferDirection {
        DOWN, UP
    };

    TransferEntry(QObject *parent = 0, QWebdav *webdav = 0,
                  QString name = "", QString remotePath = "",
                  QString localPath = "", qint64 size = 0,
                  TransferDirection direction = DOWN, bool open = false);

    ~TransferEntry();

    QString getName();
    QString getLocalPath();
    QString getRemotePath();
    qint64 getSize();
    qreal getProgress();
    int getTransferDirection();

    void setProgress(qreal value);

    void startTransfer();
    Q_INVOKABLE void cancelTransfer();

private:
    QWebdav *webdav;
    QNetworkReply *networkReply;
    QFile *localFile;

    bool m_open;

    QString m_name;
    QString m_localPath;
    QString m_remotePath;
    qint64 m_size;
    qreal m_progress;
    TransferDirection m_direction;

signals:
    void nameChanged();
    void localPathChanged();
    void sizeChanged();
    void progressChanged(qreal progress, QString remotePath);
    void transferCompleted(bool success);

public slots:
    void handleReadComplete();
    void handleProgressChange(qint64 bytes, qint64 bytesTotal);

};

#endif // TRANSFERENTRY_H
