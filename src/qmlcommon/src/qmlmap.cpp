#include "qmlmap.h"

QmlMap::QmlMap(QObject *parent) : QObject(parent)
{

}

void QmlMap::insert(const QString &key, const QVariant &value)
{
    this->m_map.insert(key, value);
    Q_EMIT inserted(key);
}

bool QmlMap::contains(const QString &key)
{
    return this->m_map.contains(key);
}

QVariant QmlMap::value(const QString &key)
{
    return this->m_map.value(key);
}

int QmlMap::remove(const QString &key)
{
    int ret = this->m_map.remove(key);
    if (ret > 0)
        Q_EMIT removed(key);
    return ret;
}
