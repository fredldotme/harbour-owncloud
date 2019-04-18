#ifndef FILEPATHUTIL_H
#define FILEPATHUTIL_H

#include <QObject>
#include <QString>

#include <settings/nextcloudsettingsbase.h>

class FilePathUtil : public QObject
{
    Q_OBJECT
public:
    FilePathUtil(QObject* parent = Q_NULLPTR) : QObject(parent) {}
    Q_INVOKABLE static QString destinationFromMIME(const QString& mime);
    Q_INVOKABLE static QString destination(AccountBase* account);
    Q_INVOKABLE static QString getCanonicalPath(const QString &path);
    Q_INVOKABLE static QString getWebDavFileUrl(const QString &path,
                                                AccountBase* settings);
    Q_INVOKABLE static bool fileExists(const QString& filePath);
    Q_INVOKABLE static bool removeFile(const QString& filePath);
};

#endif // FILEPATHUTIL_H
