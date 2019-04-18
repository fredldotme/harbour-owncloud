#include "webdavmediafeeder.h"

#include <QNetworkRequest>
#include <util/webdav_utils.h>

inline QMediaPlayer* mediaPlayerObjectCast(QObject* object) {
    if (!object) {
        qWarning() << "the provided object is a nullptr";
        return Q_NULLPTR;
    }

    QMediaPlayer* mediaPlayer = qvariant_cast<QMediaPlayer*>(object->property("mediaObject"));
    if (!mediaPlayer) {
        qWarning() << "the object doesn't contain a QMediaPlayer*";
        qWarning() << "is mediaObject null?" << object->property("mediaObject").isNull();
        return Q_NULLPTR;
    }
    return mediaPlayer;
}

inline bool objectContainsQMediaPlayer(QObject* object) {
    QMediaPlayer* mediaPlayer = mediaPlayerObjectCast(object);
    return (mediaPlayer != Q_NULLPTR);
}

WebDavMediaFeeder::WebDavMediaFeeder(
        QObject* parent,
        QObject* mediaPlayer,
        AccountBase* settings) :
    QObject(parent),
    m_mediaPlayer(mediaPlayer),
    m_settings(settings)
{
}

WebDavMediaFeeder::~WebDavMediaFeeder()
{
    qDebug() << Q_FUNC_INFO;
    stop();
}

void WebDavMediaFeeder::play()
{
    QMediaPlayer* providedMediaPlayer =
            mediaPlayerObjectCast(this->m_mediaPlayer);

    if (!providedMediaPlayer)
        return;

    const QNetworkRequest request = getOcsRequest(QNetworkRequest(this->m_url),
                                                  this->m_settings);
    if (providedMediaPlayer->state() == QMediaPlayer::StoppedState) {
        providedMediaPlayer->setMedia(QMediaContent(request));
    }
    providedMediaPlayer->play();
}

void WebDavMediaFeeder::pause()
{
    QMediaPlayer* providedMediaPlayer =
            mediaPlayerObjectCast(this->m_mediaPlayer);

    if (!providedMediaPlayer)
        return;

    providedMediaPlayer->pause();
}

void WebDavMediaFeeder::stop()
{
    QMediaPlayer* providedMediaPlayer =
            mediaPlayerObjectCast(this->m_mediaPlayer);

    if (!providedMediaPlayer)
        return;

    providedMediaPlayer->stop();
    providedMediaPlayer->setMedia(QMediaContent());
}

void WebDavMediaFeeder::setMediaPlayer(QObject* object)
{
    if (!objectContainsQMediaPlayer(object))
        return;

    if (this->m_mediaPlayer == object)
        return;

    this->m_mediaPlayer = object;
    Q_EMIT mediaPlayerChanged();
}

QObject* WebDavMediaFeeder::mediaPlayer()
{
    return this->m_mediaPlayer;
}

void WebDavMediaFeeder::setUrl(const QUrl &url)
{
    if (this->m_url == url)
        return;

    this->m_url = url;
    Q_EMIT urlChanged();
}

QUrl WebDavMediaFeeder::url()
{
    return this->m_url;
}

AccountBase* WebDavMediaFeeder::settings()
{
    return this->m_settings;
}

void WebDavMediaFeeder::setSettings(AccountBase *v)
{
    if (this->m_settings == v)
        return;

    this->m_settings = v;
    Q_EMIT settingsChanged();
}
