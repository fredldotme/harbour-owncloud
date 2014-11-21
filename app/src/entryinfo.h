#ifndef ENTRYINFO_H
#define ENTRYINFO_H

#include <QObject>

class EntryInfo : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name)
    Q_PROPERTY(bool isDirectory READ isDirectory)
    Q_PROPERTY(qint64 size READ size)

public:
    EntryInfo(QObject *parent = 0);

    void setName(QString value);
    void setDirectory(bool value);
    void setSize(qint64 value);

    QString name();
    bool isDirectory();
    qint64 size();

private:
    QString m_name;
    bool m_isDirectory;
    qint64 m_size;


signals:

public slots:

};

#endif // ENTRYINFO_H
