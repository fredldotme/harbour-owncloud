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
    Q_INVOKABLE void goToParentPath();
    Q_INVOKABLE void getDirectoryContent(QString path);
    Q_INVOKABLE void makeDirectory(QString dirName);
    Q_INVOKABLE void remove(QString name, bool refresh);
    Q_INVOKABLE void resetWebdav();

    QWebdav* getWebdav();
    QWebdav* getNewWebdav();

private:
    Settings *settings;
    QWebdav *webdav;
    QWebdavDirParser parser;
    QString currentPath;
    QVariantList entries;
    QStack<QList<EntryInfo*> > entryStack;
    QMutex deleteMutex;
    bool abortIntended;

    void applySettingsToWebdav(QWebdav *webdav);

signals:
    void directoryContentChanged(QString currentPath, QVariantList entries);
    void sslCertifcateError(QString md5Digest, QString sha1Digest);
    void loginFailed();
    void loginSucceeded();
    void refreshStarted(QString pathToRefresh);

public slots:
    void handleResponse();
    void printError(QString errorMsg);
    void reloadSettings();
    void testConnectionFinished();
    void proxyHandleSslError(const QList<QSslError> &errors);
    void proxyHandleLoginFailed();
    void refreshDirectoryContent();

public:
    void start();

};

#endif // OWNCLOUDBROWSER_H
