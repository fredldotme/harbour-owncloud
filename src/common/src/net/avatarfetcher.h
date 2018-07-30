#ifndef AVATARFETCHER_H
#define AVATARFETCHER_H

#include <QObject>
#include "abstractfetcher.h"

class AvatarFetcher : public AbstractFetcher
{
    Q_OBJECT

public:
    explicit AvatarFetcher(QObject *parent = nullptr);

public slots:
    void fetch() Q_DECL_OVERRIDE;

};

#endif // AVATARFETCHER_H
