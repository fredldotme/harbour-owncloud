#include "intentfileselectorreceiver.h"

#include <QAndroidJniObject>
#include <QDebug>

IntentFileSelectorReceiver::IntentFileSelectorReceiver(QObject *parent) : QObject(parent)
{

}

QString IntentFileSelectorReceiver::resolveUri(const QString& uri)
{
    QAndroidJniObject jUrl = QAndroidJniObject::fromString(uri);
    QAndroidJniObject resolvedUri =
            QAndroidJniObject::callStaticObjectMethod(
                "me/fredl/ghostcloud/ShareUtil",
                "getFilePathForContent",
                "(Ljava/lang/String;)V",
                jUrl.object<jstring>());

    const QString resolvedUriString = resolvedUri.toString();
    qDebug() << "RESOLVED URI:" << resolvedUriString;

    return resolvedUriString;
}

void IntentFileSelectorReceiver::handleActivityResult(int receiverRequestCode,
                                                      int resultCode,
                                                      const QAndroidJniObject &data)
{
    if (receiverRequestCode != GET_FILE_REQUEST_CODE) {
        qWarning() << "Request code" << resultCode << "received";
        return;
    }

    if (resultCode == 0) {
        qWarning() << "Invalid result code";
        return;
    }

    const QAndroidJniObject content =
            data.callObjectMethod<jstring>("getDataString");
    const QString filePath = resolveUri(content.toString());

    qDebug() << "content: " << content.toString();
    qDebug() << "file path: " << filePath;
    Q_EMIT filePathReceived(filePath);
}
