#ifndef WEBDAVMEDIAFEEDER_H
#define WEBDAVMEDIAFEEDER_H

#include <QObject>

class WebDavMediaFeeder : public QObject
{
    Q_OBJECT
public:
    explicit WebDavMediaFeeder(QObject *parent = nullptr);

signals:

public slots:
};

#endif // WEBDAVMEDIAFEEDER_H