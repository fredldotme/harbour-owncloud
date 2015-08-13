#ifndef DBUSHANDLER_H
#define DBUSHANDLER_H

#include <QDBusAbstractAdaptor>

class DBusHandler : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.github.beidl.HarbourOwncloud.Daemon.Uploader")
    Q_PROPERTY(bool online READ online NOTIFY onlineChanged(bool))
    Q_PROPERTY(bool uploading READ uploading NOTIFY uploadingChanged(bool))

public:
    explicit DBusHandler(QObject *parent = 0);
    ~DBusHandler();
    static DBusHandler* instance();

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
