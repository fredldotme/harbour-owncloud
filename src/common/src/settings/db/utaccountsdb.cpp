#include "utaccountsdb.h"

#include <QDebug>

using namespace OnlineAccounts;

UtAccountsDb::UtAccountsDb(QObject *parent) : AccountsDbInterface(parent)
{
    QString applicationId;
    QStringList parts = QString::fromUtf8(qgetenv("APP_ID")).split('_');
    if (parts.count() == 3) {
        applicationId = QStringList(parts.mid(0, 2)).join('_');
    } else {
        qWarning() << "Ubuntu.OnlineAccounts: No APP_ID defined "
                      "and no applicationId given!";
    }

    this->m_manager = new Manager(applicationId, this);
    this->m_manager->waitForReady();
    qInfo() << "Available services: ";
    for (Service service : this->m_manager->availableServices()) {
        qInfo() << "\t" << service.id() << ":" << service.displayName();
    }
}

void UtAccountsDb::refresh()
{

}
