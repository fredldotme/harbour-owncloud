#ifndef OCSSHARELISTCOMMANDENTITY_H
#define OCSSHARELISTCOMMANDENTITY_H

#include <commandentity.h>

class OcsShareListCommandEntity : public QObject
{
    Q_OBJECT
public:
    explicit OcsShareListCommandEntity(QObject *parent = nullptr);

signals:

public slots:
};

#endif // OCSSHARELISTCOMMANDENTITY_H
