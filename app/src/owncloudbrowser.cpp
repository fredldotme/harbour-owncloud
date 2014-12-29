#include "owncloudbrowser.h"

OwnCloudBrowser::OwnCloudBrowser(QObject *parent, Settings *settings) :
    QObject(parent)
{
    this->webdav = 0;
    this->settings = settings;
    this->abortIntended = false;
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

QWebdav* OwnCloudBrowser::getNewWebdav()
{
    /* Used for file uploads
     * Helps to not confuse error signals of simultaneous file operations */
    QWebdav* newWebdav = new QWebdav();
    connect(newWebdav, SIGNAL(finished(QNetworkReply*)), newWebdav, SLOT(deleteLater()), Qt::DirectConnection);

    applySettingsToWebdav(newWebdav);
    return newWebdav;
}

void OwnCloudBrowser::reloadSettings()
{
    applySettingsToWebdav(webdav);
}

void OwnCloudBrowser::applySettingsToWebdav(QWebdav *webdav)
{
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
    if(!abortIntended) {
        qDebug() << "BEIDL Failed";
        disconnect(&parser, SIGNAL(finished()), this, SLOT(handleResponse()));

        emit loginFailed();
    } else {
        abortIntended = false;
    }
}

void OwnCloudBrowser::handleResponse()
{
    QList<QWebdavItem> list = parser.getList();

    entries.clear();
    QList<EntryInfo*> deletables;

    deleteMutex.lock();
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

        deletables.append(entry);
    }
    entryStack.push(deletables);
    emit directoryContentChanged(parser.path(), entries);
    deleteMutex.unlock();
}

void OwnCloudBrowser::printError(QString msg)
{
    qDebug() << "ERROR: " << msg;
}

QString OwnCloudBrowser::getCurrentPath()
{
    return currentPath;
}

void OwnCloudBrowser::goToParentPath()
{
    // Called when navigating back in the browser
    // Keeping path and UI in sync

    QString tmpPath = currentPath.mid(0, currentPath.length() - 1);
    currentPath = tmpPath.mid(0, tmpPath.lastIndexOf('/') + 1);

    abortIntended = true;
    bool busy = parser.isBusy();
    parser.abort();

    deleteMutex.lock();
    if(!busy) {
        QList<EntryInfo*> deletables = entryStack.pop();
        for(int i = 0; i < deletables.length(); i++) {
            deletables.at(i)->deleteLater();
        }
        deletables.clear();
    }
    deleteMutex.unlock();
}

void OwnCloudBrowser::getDirectoryContent(QString path)
{
    currentPath = path;
    parser.listDirectory(webdav, path);
}

void OwnCloudBrowser::refreshDirectoryContent()
{
    parser.listDirectory(webdav, currentPath);
}

void OwnCloudBrowser::makeDirectory(QString dirName)
{
    QWebdav* mkdirWebdav = getNewWebdav();
    connect(mkdirWebdav, SIGNAL(finished(QNetworkReply*)), this, SLOT(refreshDirectoryContent()), Qt::DirectConnection);
    connect(mkdirWebdav, SIGNAL(finished(QNetworkReply*)), mkdirWebdav, SLOT(deleteLater()), Qt::DirectConnection);
    mkdirWebdav->mkdir(currentPath + dirName);

    emit refreshStarted(currentPath);
}

void OwnCloudBrowser::remove(QString name, bool refresh)
{
    qDebug() << "Removing " << name;
    QWebdav* rmWebdav = getNewWebdav();
    if(refresh)
        connect(rmWebdav, SIGNAL(finished(QNetworkReply*)), this, SLOT(refreshDirectoryContent()), Qt::DirectConnection);
    connect(rmWebdav, SIGNAL(finished(QNetworkReply*)), rmWebdav, SLOT(deleteLater()), Qt::DirectConnection);
    rmWebdav->remove(name);

    if(refresh)
        emit refreshStarted(currentPath);
}
