#include "ocscommandentity.h"

OcsCommandEntity::OcsCommandEntity(QObject *parent,
                                   QString path,
                                   QMap<QByteArray, QByteArray> headers,
                                   AccountBase* settings) :
    HttpGetCommandEntity(parent, path, prepareOcsHeaders(settings, headers), settings)
{

}
