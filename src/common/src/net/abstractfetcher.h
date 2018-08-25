#ifndef ABSTRACTFETCHER_H
#define ABSTRACTFETCHER_H

#include <QObject>
#include <provider/storage/cloudstorageprovider.h>
#include <cacheprovider.h>
#include <qwebdav.h>

class AbstractFetcher : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(CacheProvider* cacheProvider READ cacheProvider WRITE setCacheProvider NOTIFY cacheProviderChanged)
    Q_PROPERTY(CloudStorageProvider* commandQueue READ commandQueue WRITE setCommandQueue NOTIFY commandQueueChanged)
    Q_PROPERTY(int width READ width WRITE setWidth NOTIFY widthChanged)
    Q_PROPERTY(int height READ height WRITE setHeight NOTIFY heightChanged)
    Q_PROPERTY(bool fetching READ fetching NOTIFY fetchingChanged)

public:
    explicit AbstractFetcher(QObject *parent = Q_NULLPTR);

    CacheProvider* cacheProvider();
    void setCacheProvider(CacheProvider* v);
    CloudStorageProvider* commandQueue();
    void setCommandQueue(CloudStorageProvider* v);

public slots:
    virtual void fetch() = 0;

protected:
    QString source();
    void setSource(const QString& v);
    int width();
    void setWidth(int v);
    int height();
    void setHeight(int v);
    bool fetching();
    void setFetching(bool v);

private:
    QString m_source;
    int m_width = 0;
    int m_height = 0;
    bool m_fetching = false;
    CloudStorageProvider* m_commandQueue = Q_NULLPTR;
    CacheProvider* m_cacheProvider = Q_NULLPTR;

signals:
    void sourceChanged();
    void cacheProviderChanged();
    void commandQueueChanged();
    void widthChanged();
    void heightChanged();
    void fetchingChanged();
};
Q_DECLARE_METATYPE(AbstractFetcher*)

#endif // ABSTRACTFETCHER_H
