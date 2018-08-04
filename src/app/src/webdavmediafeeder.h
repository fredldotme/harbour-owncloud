#ifndef WEBDAVMEDIAFEEDER_H
#define WEBDAVMEDIAFEEDER_H

#include <QObject>
#include <QMediaPlayer>
#include <settings/nextcloudsettingsbase.h>

class WebDavMediaFeeder : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QObject* mediaPlayer READ mediaPlayer
               WRITE setMediaPlayer
               NOTIFY mediaPlayerChanged)
    Q_PROPERTY(QUrl url READ url WRITE setUrl NOTIFY urlChanged)
    Q_PROPERTY(NextcloudSettingsBase* settings READ settings WRITE setSettings NOTIFY settingsChanged)

public:
    explicit WebDavMediaFeeder(
            QObject* parent = Q_NULLPTR,
            QObject* mediaPlayer = Q_NULLPTR,
            NextcloudSettingsBase* settings = Q_NULLPTR);
    ~WebDavMediaFeeder();

    void setMediaPlayer(QObject* object);
    QObject* mediaPlayer();
    void setUrl(const QUrl& url);
    QUrl url();
    NextcloudSettingsBase* settings();
    void setSettings(NextcloudSettingsBase* v);

public slots:
    void play();
    void pause();
    void stop();

private:
    QObject* m_mediaPlayer = Q_NULLPTR;
    NextcloudSettingsBase* m_settings = Q_NULLPTR;
    QUrl m_url;

signals:
    void mediaPlayerChanged();
    void urlChanged();
    void settingsChanged();
};

#endif // WEBDAVMEDIAFEEDER_H
