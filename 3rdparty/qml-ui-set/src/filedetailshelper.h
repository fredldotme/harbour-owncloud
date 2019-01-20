#ifndef FILEDETAILSHELPER_H
#define FILEDETAILSHELPER_H

#include <QObject>

class FileDetailsHelper : public QObject
{
    Q_OBJECT
public:
    explicit FileDetailsHelper(QObject *parent = 0);

public slots:
    QString getHRSize(quint64 bytes);
    QString getHRSizeFromString(QString byteString);
    QString getIconFromMime(const QString& mimeType);
};

#endif // FILEDETAILSHELPER_H
