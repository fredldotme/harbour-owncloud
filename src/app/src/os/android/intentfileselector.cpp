#include "intentfileselector.h"

#include <QAndroidIntent>
#include <QtAndroid>

IntentFileSelector::IntentFileSelector(QObject *parent) : QObject(parent)
{
    QObject::connect(&this->m_resultReceiver, &IntentFileSelectorReceiver::filePathReceived,
                     this, &IntentFileSelector::fileSelected,
                     Qt::QueuedConnection);
}

void IntentFileSelector::fileSelectIntent()
{
    QAndroidJniObject ACTION_GET_CONTENT =
            QAndroidJniObject::fromString("android.intent.action.GET_CONTENT");
    QAndroidJniObject intent("android/content/Intent");

    if (!(ACTION_GET_CONTENT.isValid() && intent.isValid())) {
        return;
    }

    intent.callObjectMethod("setAction", "(Ljava/lang/String;)Landroid/content/Intent;",
                            ACTION_GET_CONTENT.object<jstring>());
    intent.callObjectMethod("setType", "(Ljava/lang/String;)Landroid/content/Intent;",
                            QAndroidJniObject::fromString("*/*").object<jstring>());

    QtAndroid::startActivity(intent.object<jobject>(),
                             IntentFileSelectorReceiver::GET_FILE_REQUEST_CODE,
                             &this->m_resultReceiver);
}
