#include "dbushandler.h"
#include <QDebug>
#include <QDBusConnection>
#include <QDBusMessage>

DBusHandler::DBusHandler(QObject *parent) : QObject(parent)
{
}

DBusHandler::~DBusHandler()
{

}

void DBusHandler::requestConfig() {
    message().setDelayedReply(true);
    emit dialogRequested(message());
}
