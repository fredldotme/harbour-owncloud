#ifndef INTENTFILESELECTORRECEIVER_H
#define INTENTFILESELECTORRECEIVER_H

#include <QObject>
#include <QAndroidActivityResultReceiver>

class IntentFileSelectorReceiver :
        public QObject,
        public QAndroidActivityResultReceiver
{
    Q_OBJECT
public:
    const static int GET_FILE_REQUEST_CODE = 200;

    explicit IntentFileSelectorReceiver(QObject *parent = nullptr);
    virtual void handleActivityResult(int receiverRequestCode,
                                      int resultCode,
                                      const QAndroidJniObject &data) Q_DECL_OVERRIDE;

private:
    QString resolveUri(const QString& uri);

signals:
    void filePathReceived(const QString& filePath);

};

#endif // INTENTFILESELECTORRECEIVER_H
