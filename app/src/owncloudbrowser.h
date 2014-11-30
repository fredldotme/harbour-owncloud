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
    OwnCloudBrowser(QObject* parent = 0, Settings* settings = 0, QWebdav* webdav = 0);

    Q_INVOKABLE QString getCurrentPath();
    Q_INVOKABLE void getDirectoryContent(QString path);

private:
    Settings *settings;
    QWebdav *webdav;
    QWebdavDirParser parser;
    QString currentPath;

    bool ignoreFail;

signals:
    void directoryContentChanged(QString currentPath, QVariantList entries);
    void sslCertifcateError(QString md5Digest, QString sha1Digest);
    void loginFailed();
    void loginSucceeded();

public slots:
    void handleResponse();
    void printError(QString errorMsg);
    void reloadSettings();
    void proxyHandleSslError(const QList<QSslError> &errors);
    void proxyHandleLoginFailed();

public:
    void start();

};

#endif // OWNCLOUDBROWSER_H
