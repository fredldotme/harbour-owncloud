#ifndef OWNCLOUDBROWSER_H
#define OWNCLOUDBROWSER_H

#include <QObject>

#include <net/webdav_utils.h>
#include <qwebdavdirparser.h>
#include <qwebdavitem.h>
#include <QVariant>

#include <net/remotefileinfo.h>
#include <settings/nextcloudsettingsbase.h>

class OwnCloudBrowser : public QObject
{
    Q_OBJECT

public:
    Q_PROPERTY(NextcloudSettingsBase* settings READ getSettings WRITE setSettings NOTIFY settingsObjChanged)

    OwnCloudBrowser(QObject* parent = 0, NextcloudSettingsBase* settings = nullptr);
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
    Q_INVOKABLE NextcloudSettingsBase* getSettings();
    Q_INVOKABLE void setSettings(NextcloudSettingsBase* settings);

    QWebdav* getWebdav();

private:
    NextcloudSettingsBase *settings;
    QWebdav *webdav;
    QWebdavDirParser parser;
    QString currentPath;
    QVariantList entries;
    QStack<QList<RemoteFileInfo*> > entryStack;
    QMutex deleteMutex;
    bool abortIntended;

signals:
    void directoryContentChanged(QString currentPath, QVariantList entries);
    void sslCertifcateError(QString md5Digest, QString sha1Digest);
    void loginFailed();
    void loginSucceeded();
    void refreshStarted(QString pathToRefresh);
    void settingsObjChanged();

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
Q_DECLARE_METATYPE(OwnCloudBrowser*)

#endif // OWNCLOUDBROWSER_H
