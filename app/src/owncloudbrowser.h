#ifndef OWNCLOUDBROWSER_H
#define OWNCLOUDBROWSER_H

#include <QObject>

#include <qwebdav.h>
#include <qwebdavdirparser.h>
#include <qwebdavitem.h>
#include <QVariant>

#include "entryinfo.h"

class OwnCloudBrowser : public QObject
{
    Q_OBJECT

public:
    OwnCloudBrowser(QObject* parent = 0);

    Q_INVOKABLE QString getCurrentPath();
    Q_INVOKABLE void getDirectoryContent(QString path);

private:
    QWebdav webdav;
    QWebdavDirParser parser;
    QString currentPath;

signals:
    void directoryContentChanged(QString currentPath, QVariantList entries);
    
public slots:
    void printList();
    void printError(QString errorMsg);

public:
    void start();

};

#endif // OWNCLOUDBROWSER_H
