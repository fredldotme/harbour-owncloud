#ifndef CACHEPROVIDER_H
#define CACHEPROVIDER_H

#include <settings/nextcloudsettingsbase.h>
#include <QObject>
#include <QTimer>
#include <QFile>

class CacheProvider : public QObject
{
    Q_OBJECT
public:
    explicit CacheProvider(QObject *parent = Q_NULLPTR,
                           AccountBase* account = Q_NULLPTR);

    bool cacheFileExists(const QString& identifier);
    bool isFileCurrent(const QString& identifier);
    QFile* getCacheFile(const QString& identifier, QFile::OpenMode mode);
    QString getPathForIdentifier(const QString& identifier);

public slots:
    void clearCache();
    void clearDownloads();

private:
    bool clearPath(const QString& path);

    QTimer m_clearTimer;
    QString m_cacheDir;
    QString m_downloadDir;

signals:
    void cacheCleared();
};

#endif // CACHEPROVIDER_H
