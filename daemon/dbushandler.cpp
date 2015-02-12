#include "dbushandler.h"
#include <QDebug>
DBusHandler::DBusHandler(QObject *parent) : QDBusAbstractAdaptor(parent),
    m_suspended(false)
{
}

DBusHandler::~DBusHandler()
{

}

void DBusHandler::setSuspended(bool suspended) {
    qDebug() << Q_FUNC_INFO << "setting suspended" << suspended;
    if (suspended == m_suspended) return;
    m_suspended = suspended;
    emit suspendedChanged(suspended);
}

void DBusHandler::setOnline(bool online)
{
    qDebug() << Q_FUNC_INFO << "setting online" << online;
    if (online == m_online) return;
    m_online = online;
    emit onlineChanged(online);
}

void DBusHandler::setUploading(bool uploading)
{
    qDebug() << Q_FUNC_INFO << "setting uploading" << uploading;
    if (uploading == m_uploading) return;
    m_uploading = uploading;
    emit uploadingChanged(uploading);
}

