#ifndef OWNCLOUDBROWSER_H
#define OWNCLOUDBROWSER_H

#include <QObject>

#include <qwebdav.h>
#include <qwebdavdirparser.h>
#include <qwebdavitem.h>
#include <QVariant>

#include "entryinfo.h"
#include "settings.h"

class OwnCloudBrowser : public QObject
{
    Q_OBJECT

public:
    OwnCloudBrowser(QObject* parent = 0, Settings* settings = 0);
    ~OwnCloudBrowser();

    Q_INVOKABLE void testConnection();
    Q_INVOKABLE QString getCurrentPath();
    Q_INVOKABLE void getDirectoryContent(QString path);
    Q_INVOKABLE void resetWebdav();

    QWebdav* getWebdav();

private:
    Settings *settings;
    QWebdav *webdav;
    QWebdavDirParser parser;
    QString currentPath;

signals:
    void directoryContentChanged(QString currentPath, QVariantList entries);
    void sslCertifcateError(QString md5Digest, QString sha1Digest);
    void loginFailed();
    void loginSucceeded();

public slots:
    void handleResponse();
    void printError(QString errorMsg);
    void reloadSettings();
    void testConnectionFinished();
    void proxyHandleSslError(const QList<QSslError> &errors);
    void proxyHandleLoginFailed();

public:
    void start();

};

#endif // OWNCLOUDBROWSER_H
