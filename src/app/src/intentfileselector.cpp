#include "intentfileselector.h"

#include <QAndroidIntent>

IntentFileSelector::IntentFileSelector(QObject *parent) : QObject(parent)
{

}

void IntentFileSelector::fileSelectIntent()
{
    QAndroidIntent intent = QAndroidIntent("ACTION_GET_CONTENT");
}
