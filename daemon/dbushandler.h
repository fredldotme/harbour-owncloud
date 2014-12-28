#ifndef DBUSHANDLER_H
#define DBUSHANDLER_H

#include <QDBusAbstractAdaptor>

class DBusHandler : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.github.beidl.HarbourOwncloud.Daemon.Uploader")
    Q_PROPERTY(bool suspended READ suspended WRITE setSuspended NOTIFY suspendedChanged(bool))
    Q_PROPERTY(bool offline READ offline NOTIFY offlineChanged(bool))

public:
    explicit DBusHandler(QObject *parent);
    ~DBusHandler();

public slots:
    void setSuspended(bool suspended);
    bool suspended() { return m_suspended; }

    void setOffline(bool offline);
    bool offline() { return m_offline; }

    void reloadConfig() { emit configChanged(); }

signals:
    void fileUploaded(QString filePath);
    void suspendedChanged(bool suspended);
    void offlineChanged(bool offline);
    void uploadError(QString errorMessage);
    void connectError(QString errorMessage);
    void configChanged();

private:
    bool m_suspended;
    bool m_offline;
};

#endif // DBUSHANDLER_H
