#ifndef OWNCLOUDBROWSER_H
#define OWNCLOUDBROWSER_H

#include <QObject>

#include <webdav_utils.h>
#include <qwebdavdirparser.h>
#include <qwebdavitem.h>
#include <QVariant>

#include <entryinfo.h>

class OwnCloudBrowser : public QObject
{
    Q_OBJECT

public:
    Q_PROPERTY(Settings* settings READ getSettings)

    OwnCloudBrowser(QObject* parent = 0, Settings* settings = Settings::instance());
    ~OwnCloudBrowser();

    Q_INVOKABLE void testConnection();
    Q_INVOKABLE QString getCurrentPath();
    Q_INVOKABLE void goToParentPath();
    Q_INVOKABLE void getDirectoryContent(QString path);
    Q_INVOKABLE void makeDirectory(QString dirName);
    Q_INVOKABLE void remove(QString name, bool refresh);
    Q_INVOKABLE void move(QString from, QString to, bool refresh);
    Q_INVOKABLE void copy(QString from, QString to, bool refresh);
    Q_INVOKABLE void resetWebdav();
    Q_INVOKABLE QString getCanonicalPath(const QString& path);
    Q_INVOKABLE Settings* getSettings();

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
