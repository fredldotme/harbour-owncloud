#ifndef OWNCLOUDBROWSER_H
#define OWNCLOUDBROWSER_H

#include <QObject>

#include <net/webdav_utils.h>
#include <qwebdavdirparser.h>
#include <qwebdavitem.h>
#include <QVariant>
#include <settings/nextcloudsettingsbase.h>
#include <commands/webdavcommandentity.h>

class OwnCloudBrowser : public QObject
{
    Q_OBJECT

public:
    Q_PROPERTY(NextcloudSettingsBase* settings READ getSettings WRITE setSettings NOTIFY settingsObjChanged)

    OwnCloudBrowser(QObject* parent = 0, NextcloudSettingsBase* settings = nullptr);
    ~OwnCloudBrowser();

    Q_INVOKABLE void testConnection();
    Q_INVOKABLE QString getCurrentPath();
    Q_INVOKABLE void getDirectoryContent(QString path);
    Q_INVOKABLE QList<CommandEntity*> fileDownloadRequest(QString from,
                                                          QString mimeType = QStringLiteral(""),
                                                          bool open = false);
    Q_INVOKABLE QList<CommandEntity*> fileUploadRequest(QString from, QString to);
    Q_INVOKABLE CommandEntity* makeDirectoryRequest(QString dirName);
    Q_INVOKABLE CommandEntity* removeRequest(QString name, bool refresh);
    Q_INVOKABLE CommandEntity* moveRequest(QString from, QString to, bool refresh);
    Q_INVOKABLE CommandEntity* copyRequest(QString from, QString to, bool refresh);
    Q_INVOKABLE CommandEntity* directoryListingRequest(QString path);
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
