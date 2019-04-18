#ifndef QMLMAP_H
#define QMLMAP_H

#include <QObject>
#include <QMap>
#include <QVariant>

class QmlMap : public QObject
{
    Q_OBJECT
public:
    explicit QmlMap(QObject *parent = Q_NULLPTR);

public slots:
    void insert(const QString& key, const QVariant& value);
    bool contains(const QString& key);
    QVariant value(const QString& key);
    int remove(const QString& key);
    void clear();

private:
    QMap<QString, QVariant> m_map;

signals:
    void inserted(QString key);
    void removed(QString key);
    void cleared();

};
Q_DECLARE_METATYPE(QmlMap*)

#endif // QMLMAP_H
