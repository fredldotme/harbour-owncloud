#include "examinationsettings.h"

ExaminationSettings::ExaminationSettings(QObject *parent) :
    NextcloudSettingsBase(parent)
{

}

bool ExaminationSettings::setHostString(QString host)
{
    if (parseFromAddressString(host)) {
        this->m_hoststring = host;
        return true;
    }
    return false;
}
