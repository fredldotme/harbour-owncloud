#ifndef THUMBNAILFETCHER_H
#define THUMBNAILFETCHER_H

#include <QObject>
#include <commandqueue.h>
#include <settings/nextcloudsettingsbase.h>

class ThumbnailFetcher : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(NextcloudSettingsBase* settings READ settings WRITE setSettings NOTIFY settingsChanged)
    Q_PROPERTY(CommandQueue* commandQueue READ commandQueue WRITE setCommandQueue NOTIFY commandQueueChanged)
    Q_PROPERTY(qint8 width READ width WRITE setWidth NOTIFY widthChanged)
    Q_PROPERTY(qint8 height READ height WRITE setHeight NOTIFY heightChanged)
    Q_PROPERTY(bool fetching READ fetching NOTIFY fetchingChanged)

public:
    explicit ThumbnailFetcher(QObject *parent = Q_NULLPTR);

public slots:
    void fetchThumbnail(QString remoteFile);

private:
    NextcloudSettingsBase* settings();
    void setSettings(NextcloudSettingsBase* v);
    CommandQueue* commandQueue();
    void setCommandQueue(CommandQueue* v);
    QString source();
    void setSource(const QString& v);
    qint8 width();
    void setWidth(qint8 v);
    qint8 height();
    void setHeight(qint8 v);
    bool fetching();

    QString m_source;
    qint8 m_width = 0;
    qint8 m_height = 0;
    NextcloudSettingsBase* m_settings = Q_NULLPTR;
    CommandQueue* m_commandQueue = Q_NULLPTR;

signals:
    void sourceChanged();
    void settingsChanged();
    void commandQueueChanged();
    void widthChanged();
    void heightChanged();
    void fetchingChanged();
};
Q_DECLARE_METATYPE(ThumbnailFetcher*)

#endif // THUMBNAILFETCHER_H
