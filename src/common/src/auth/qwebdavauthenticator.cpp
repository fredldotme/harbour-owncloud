#include "qwebdavauthenticator.h"

#include <nextcloudendpointconsts.h>
#include <net/webdav_utils.h>

#include <QDebug>

QWebDavAuthenticator::QWebDavAuthenticator(QObject *parent,
                                           NextcloudSettingsBase* settings) :
    Authenticator(parent, settings)
{
    QObject::connect(&this->m_dirParser, &QWebdavDirParser::errorChanged,
                     this, [=](QString error) {
        qWarning() << "authentication failed due to" << error;
        Q_EMIT authenticationFailed();
    });
}

void QWebDavAuthenticator::authenticate(bool saveCredentials)
{
    qDebug() << Q_FUNC_INFO;

    this->m_dirParser.abort();

    // Apply settings to new or existing QWebdav object
    updateClientSettings();

    if (!this->m_client) {
        qWarning() << "Invalid QWebdav client object created";
        Q_EMIT authenticationFailed();
        return;
    }

    QObject::connect(this->m_client, &QWebdav::checkSslCertifcate,
                     this, &QWebDavAuthenticator::sslErrorOccured);
    QObject::connect(this->m_client, &QNetworkAccessManager::finished,
                     this, &QWebDavAuthenticator::testConnectionFinished);

    this->m_saveCredentials = saveCredentials;
    this->m_dirParser.listDirectory(this->m_client, "/");

    setRunning(true);
}

void QWebDavAuthenticator::updateClientSettings()
{
    if (!this->settings()) {
        qWarning() << "No settings object provided";
        return;
    }

    // Apply settings to new or existing QWebdav object
    if (!this->m_client) {
        this->m_client = getNewWebDav(this->settings(), NEXTCLOUD_ENDPOINT_WEBDAV, this);
    } else {
        applySettingsToWebdav(this->settings(), this->m_client, NEXTCLOUD_ENDPOINT_WEBDAV);
    }
}

void QWebDavAuthenticator::sslErrorOccured(const QList<QSslError> &errors)
{
    setRunning(false);

    if (this->m_client && errors.length() > 0) {
        QSslCertificate cert = errors[0].certificate();
        Q_EMIT sslError(this->m_client->
                        digestToHex(cert.digest(QCryptographicHash::Md5)),
                        this->m_client->
                        digestToHex(cert.digest(QCryptographicHash::Sha1)));
    }

    Q_EMIT authenticationFailed();
}

void QWebDavAuthenticator::testConnectionFinished(QNetworkReply *reply)
{
    setRunning(false);

    if (!reply || reply->error() != QNetworkReply::NoError) {
        Q_EMIT authenticationFailed();
        return;
    }

    if (this->m_saveCredentials && this->settings()) {
        this->settings()->writeSettings();
    }
    Q_EMIT authenticationSuccessful();
}
