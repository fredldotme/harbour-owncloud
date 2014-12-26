#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <QObject>
#include <QFileSystemWatcher>
#include <QSet>

class Filesystem : public QObject
{
    Q_OBJECT

public:
    Filesystem();

signals:
    void fileFound(QString fullPath);

private slots:
    void localPathChanged();
    void scan(QString dirPath, bool recursive = false);

private:
    void rescan();

    QFileSystemWatcher m_watcher;
    QString m_localPath;
    QSet<QString> m_existingFiles;
};

#endif//FILESYSTEM_H
