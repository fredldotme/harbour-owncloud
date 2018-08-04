#include "ocscommandentity.h"

OcsCommandEntity::OcsCommandEntity(QObject *parent,
                                   QString path,
                                   QMap<QByteArray, QByteArray> headers,
                                   NextcloudSettingsBase* settings) :
    HttpGetCommandEntity(parent, path, prepareOcsHeaders(settings, headers), settings)
{

}
