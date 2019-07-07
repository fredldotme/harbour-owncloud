#ifndef COMMANDUTIL_H
#define COMMANDUTIL_H

#include <QObject>

class CommandUtil : public QObject
{
    Q_OBJECT
public:
    explicit CommandUtil(QObject *parent = Q_NULLPTR);

signals:

public slots:
};

#endif // COMMANDUTIL_H
