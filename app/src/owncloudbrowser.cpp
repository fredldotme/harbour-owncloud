#include "owncloudbrowser.h"

OwnCloudBrowser::OwnCloudBrowser(QObject *parent, Settings *settings) :
    QObject(parent)
{
    this->webdav = 0;
    this->settings = settings;
    connect(settings, SIGNAL(settingsChanged()), this, SLOT(reloadSettings()));

    resetWebdav();
}

OwnCloudBrowser::~OwnCloudBrowser()
{
    if(webdav)
        delete webdav;
}

QWebdav* OwnCloudBrowser::getWebdav()
{
    return webdav;
}

QWebdav* OwnCloudBrowser::getNewWebdav()
{
    /* Used for file uploads
     * Helps to not confuse errors of simultaneous file operations */
    QWebdav* newWebdav = new QWebdav();
    newWebdav->setConnectionSettings(settings->isHttps() ? QWebdav::HTTPS : QWebdav::HTTP,
                                 settings->hostname(),
                                 settings->path() + "remote.php/webdav",
                                 settings->username(),
                                 settings->password(),
                                 settings->port(),
                                 settings->isHttps() ? settings->md5Hex() : "",
                                 settings->isHttps() ? settings->sha1Hex() : "");
    connect(newWebdav, SIGNAL(destroyed()), this, SLOT(remoteSecondaryWebdav(newWebdav)), Qt::DirectConnection);
    secondaryWebdavs.append(newWebdav);
    return newWebdav;
}

void OwnCloudBrowser::removeSecondaryWebdav(QWebdav *webdav)
{
    secondaryWebdavs.removeOne(webdav);
}

void OwnCloudBrowser::resetWebdav()
{
    if(webdav) {
        disconnect(webdav, SIGNAL(errorChanged(QString)), this, SLOT(proxyHandleLoginFailed()));
        disconnect(&parser, 0, 0, 0);
        delete webdav;
    }
    webdav = new QWebdav();
    connect(webdav, SIGNAL(errorChanged(QString)), this, SLOT(proxyHandleLoginFailed()), Qt::DirectConnection);
    connect(&parser, SIGNAL(errorChanged(QString)), this, SLOT(proxyHandleLoginFailed()), Qt::DirectConnection);
}

void OwnCloudBrowser::reloadSettings() {
    webdav->setConnectionSettings(settings->isHttps() ? QWebdav::HTTPS : QWebdav::HTTP,
                                 settings->hostname(),
                                 settings->path() + "remote.php/webdav",
                                 settings->username(),
                                 settings->password(),
                                 settings->port(),
                                 settings->isHttps() ? settings->md5Hex() : "",
                                 settings->isHttps() ? settings->sha1Hex() : "");
}

void OwnCloudBrowser::testConnection()
{
    connect(webdav, SIGNAL(checkSslCertifcate(const QList<QSslError>&)), this, SLOT(proxyHandleSslError(const QList<QSslError>&)));
    connect(webdav, SIGNAL(finished(QNetworkReply*)), this, SLOT(testConnectionFinished()), Qt::DirectConnection);

    parser.listDirectory(webdav, "/");
}

void OwnCloudBrowser::testConnectionFinished()
{
    qDebug() << "BEIDL Finished";

    disconnect(webdav, SIGNAL(checkSslCertifcate(const QList<QSslError>&)), this, SLOT(proxyHandleSslError(const QList<QSslError>&)));
    disconnect(webdav, SIGNAL(finished(QNetworkReply*)), this, SLOT(testConnectionFinished()));

    connect(&parser, SIGNAL(finished()), this, SLOT(handleResponse()));
    emit loginSucceeded();
}

void OwnCloudBrowser::proxyHandleSslError(const QList<QSslError>& errors)
{
    QSslCertificate cert = errors[0].certificate();
    emit sslCertifcateError(webdav->digestToHex(cert.digest(QCryptographicHash::Md5)),
                            webdav->digestToHex(cert.digest(QCryptographicHash::Sha1)));
}

void OwnCloudBrowser::proxyHandleLoginFailed()
{
    qDebug() << "BEIDL Failed";
    disconnect(&parser, SIGNAL(finished()), this, SLOT(handleResponse()));

    emit loginFailed();
}

void OwnCloudBrowser::handleResponse()
{
    QList<QWebdavItem> list = parser.getList();
    QVariantList entries;

    QWebdavItem item;
    foreach(item, list) {
        EntryInfo *entry = new EntryInfo();
        entry->setPath(item.path());
        entry->setName(item.name());
        entry->setDirectory(item.isDir());
        entry->setSize(item.size());
        if(!item.isDir()) {
            entry->setMimeType(item.mimeType());
            entry->setModTime(item.lastModified());
        }

        QVariant tmpVariant;
        tmpVariant.setValue(entry);
        entries.append(tmpVariant);
    }

    emit directoryContentChanged(currentPath, entries);
}

void OwnCloudBrowser::printError(QString msg)
{
    qDebug() << "ERROR: " << msg;
}

QString OwnCloudBrowser::getCurrentPath()
{
    return currentPath;
}

void OwnCloudBrowser::getDirectoryContent(QString path)
{
    currentPath = path;
    parser.listDirectory(webdav, path);
}
