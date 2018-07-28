#ifndef FILEPATHUTIL_H
#define FILEPATHUTIL_H

#include <QObject>
#include <QString>

class FilePathUtil : public QObject
{
    Q_OBJECT
public:
    FilePathUtil(QObject* parent = Q_NULLPTR) : QObject(parent) {}
    Q_INVOKABLE static QString destinationFromMIME(QString mime);
    Q_INVOKABLE static QString getCanonicalPath(const QString &path);
};

#endif // FILEPATHUTIL_H
