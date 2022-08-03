#ifndef SERVICECREATOR_H
#define SERVICECREATOR_H

#include <QObject>

class ServiceCreator : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool serviceEnabled READ serviceEnabled WRITE setServiceEnabled NOTIFY serviceEnabledChanged)

public:
    explicit ServiceCreator(QObject *parent = nullptr);

private:
    bool serviceEnabled();
    void setServiceEnabled(bool value);

    QString serviceFilePath();
    QString appId();
    void initCtl(const QString& action);
    void initCtlReload();

signals:
    void serviceEnabledChanged();

};

#endif // SERVICECREATOR_H
