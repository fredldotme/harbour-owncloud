#ifndef DBUSHANDLER_H
#define DBUSHANDLER_H

#include <QObject>
#include <ownclouddbusconsts.h>

class DBusHandler : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", HARBOUR_OWNCLOUD_DAEMON_IFACE)
    Q_PROPERTY(bool online READ online NOTIFY onlineChanged)
    Q_PROPERTY(bool uploading READ uploading NOTIFY uploadingChanged)

public:
    explicit DBusHandler(QObject *parent = nullptr);
    ~DBusHandler();

public slots:
    void setOnline(bool online);
    bool online() { return m_online; }

    void setUploading(bool uploading);
    bool uploading() { return m_uploading; }

    void reloadConfig() { emit configChanged(); }

signals:
    void fileUploaded(QString filePath);
    void onlineChanged(bool online);
    void uploadingChanged(bool uploading);
    void uploadError(QString errorMessage);
    void connectError(QString errorMessage);
    void configChanged();

private:
    bool m_online;
    bool m_uploading;
};

#endif // DBUSHANDLER_H
