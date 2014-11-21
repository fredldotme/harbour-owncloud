#include "owncloudbrowser.h"

OwnCloudBrowser::OwnCloudBrowser(QObject *parent) :
    QObject(parent)
{
    // Need to decide on how to securely save password
    webdav.setConnectionSettings(QWebdav::HTTPS, "HOST", "/remote.php/webdav", "USER", "PASS", 443);
    connect(&parser, SIGNAL(finished()), this, SLOT(printList()));
    connect(&parser, SIGNAL(errorChanged(QString)), this, SLOT(printError(QString)));
    connect(&webdav, SIGNAL(errorChanged(QString)), this, SLOT(printError(QString)));
}

void OwnCloudBrowser::printList()
{
    QList<QWebdavItem> list = parser.getList();
    QVariantList entries;

    QWebdavItem item;
    foreach(item, list) {
        EntryInfo *entry = new EntryInfo();
        entry->setName(item.name());
        entry->setDirectory(item.isDir());
        entry->setSize(item.size());

        QVariant tmpVariant;
        tmpVariant.setValue(entry);
        entries.append(tmpVariant);
    }
    emit directoryContentChanged(currentPath, entries);
}

void OwnCloudBrowser::printError(QString msg)
{
    qDebug() << "ERROR: " << msg;
}

QString OwnCloudBrowser::getCurrentPath()
{
    return currentPath;
}

void OwnCloudBrowser::getDirectoryContent(QString path)
{
    currentPath = path;
    parser.listDirectory(&webdav, path);
}
