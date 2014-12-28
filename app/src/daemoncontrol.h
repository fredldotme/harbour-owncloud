#ifndef DAEMONCONTROL_H
#define DAEMONCONTROL_H

#include <QObject>

class DaemonControl : public QObject
{
    Q_OBJECT
public:
    explicit DaemonControl(QObject *parent = 0);

signals:

public slots:

};

#endif // DAEMONCONTROL_H
