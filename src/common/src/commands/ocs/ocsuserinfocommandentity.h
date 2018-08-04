#ifndef OCSUSERINFOCOMMANDENTITY_H
#define OCSUSERINFOCOMMANDENTITY_H

#include <QObject>
#include "ocscommandentity.h"

class OcsUserInfoCommandEntity : public OcsCommandEntity
{
    Q_OBJECT
public:
    explicit OcsUserInfoCommandEntity(
            QObject* parent = Q_NULLPTR,
            NextcloudSettingsBase* settings = Q_NULLPTR,
            QMap<QByteArray, QByteArray> headers = prepareOcsHeaders());

    bool startWork();
};

#endif // OCSUSERINFOCOMMANDENTITY_H
