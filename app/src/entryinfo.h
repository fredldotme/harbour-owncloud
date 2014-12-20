#ifndef ENTRYINFO_H
#define ENTRYINFO_H

#include <QObject>
#include <QDateTime>

class EntryInfo : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name NOTIFY nameChanged())
    Q_PROPERTY(QString path READ path)
    Q_PROPERTY(bool isDirectory READ isDirectory NOTIFY isDirectoryChanged())
    Q_PROPERTY(qint64 size READ size)
    Q_PROPERTY(QString mimeType READ mimeType)
    Q_PROPERTY(QDateTime modTime READ modTime)

public:
    EntryInfo(QObject *parent = 0);

    void setPath(QString value);
    void setName(QString value);
    void setDirectory(bool value);
    void setSize(qint64 value);
    void setMimeType(QString value);
    void setModTime(QDateTime modTime);

    QString path();
    QString name();
    bool isDirectory();
    qint64 size();
    QString mimeType();
    QDateTime modTime();

private:
    QString m_path;
    QString m_name;
    bool m_isDirectory;
    qint64 m_size;
    QString m_mime;
    QDateTime m_modTime;

signals:
    void nameChanged();
    void isDirectoryChanged();

public slots:

};

#endif // ENTRYINFO_H
