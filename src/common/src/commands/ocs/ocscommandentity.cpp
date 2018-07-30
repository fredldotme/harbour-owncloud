#include "ocscommandentity.h"
#include <net/webdav_utils.h>

OcsCommandEntity::OcsCommandEntity(QObject *parent,
                                   QString path,
                                   QMap<QByteArray, QByteArray> headers,
                                   NextcloudSettingsBase* settings) :
    HttpGetCommandEntity(parent, path, prepareOcsHeaders(settings, headers), settings)
{

}
