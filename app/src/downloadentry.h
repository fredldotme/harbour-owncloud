#ifndef DOWNLOADENTRY_H
#define DOWNLOADENTRY_H

#include <QObject>

class DownloadEntry : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ getName NOTIFY nameChanged)
    Q_PROPERTY(QString localPath READ getLocalPath NOTIFY localPathChanged)
    Q_PROPERTY(qint64 size READ getSize NOTIFY sizeChanged)
    Q_PROPERTY(qreal progress READ getProgress NOTIFY progressChanged)

public:
    DownloadEntry(QObject *parent = 0, QString name = "",
                  QString remotePath = "", QString localPath = "",
                  qint64 size = 0);

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

};

#endif // DOWNLOADENTRY_H
