#ifndef DAEMONCONTROL_H
#define DAEMONCONTROL_H

#include <QObject>

// Dummy implementation
class DaemonControl : public QObject
{
    Q_OBJECT
public:
    explicit DaemonControl(QObject *parent = nullptr);

public slots:
    void reloadConfig() {}
    void abort() {}

};

#endif // DAEMONCONTROL_H
