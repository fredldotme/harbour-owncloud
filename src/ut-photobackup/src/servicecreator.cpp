#include "servicecreator.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QProcess>
#include <QStandardPaths>
#include <QString>
#include <QStringList>
#include <QTextStream>
#include <QThread>

const QString serviceFileContent =
        QStringLiteral(
            "[Unit]\n"
            "Description=GhostCloud photo backups\n"
            "After=graphical.target\n"
            "[Service]\n"
            "Restart=on-failure\n"
            "Environment=LD_LIBRARY_PATH=/opt/click.ubuntu.com/me.fredl.ghostcloudphotobackup/current/usr/lib\n"
            "Environment=APP_ID=%1\n"
            "ExecStart=/opt/click.ubuntu.com/me.fredl.ghostcloudphotobackup/current/usr/bin/harbour-owncloud-daemon\n"
            "[Install]\n"
            "WantedBy=graphical-session.target\n");

static const QString initctl = QStringLiteral("/usr/bin/systemctl");

ServiceCreator::ServiceCreator(QObject *parent)
    : QObject{parent}
{

}

QString ServiceCreator::serviceFilePath()
{
    const QString upstartServiceFile =
            QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + QStringLiteral("/.config/systemd/user/ghostcloud-photobackup.service");
    return upstartServiceFile;
}

QString ServiceCreator::appId()
{
    const QStringList parts = QString::fromUtf8(qgetenv("APP_ID")).split('_');
    const QString applicationId = QStringList(parts.mid(0, 2)).join('_');
    return applicationId;
}

void ServiceCreator::initCtl(const QString& action)
{
    const QStringList args = { "--user", action, "ghostcloud-photobackup" };

    QProcess initCommand;
    initCommand.start(initctl, args);
    initCommand.waitForFinished();
}

void ServiceCreator::initCtlReload()
{
    const QStringList args = { "--user", "daemon-reload" };

    QProcess initCommand;
    initCommand.start(initctl, args);
    initCommand.waitForFinished();
}

bool ServiceCreator::serviceEnabled()
{
    return QFile::exists(serviceFilePath());
}

void ServiceCreator::setServiceEnabled(bool value)
{
    QString initAction;

    if (value) {
        {
            const QString userServices = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + QStringLiteral("/.config/systemd/user");
            QDir().mkpath(userServices);
        }
        QFile serviceFile(serviceFilePath());
        if (!serviceFile.open(QFile::WriteOnly | QFile::Text)) {
            qWarning() << "Failed to create service file";
            return;
        }

        {
            QTextStream ts(&serviceFile);
            ts << serviceFileContent.arg(appId());
            ts.flush();
        }

        initCtlReload();
        initCtl("enable");
        initCtl("start");
    } else {
        initCtl("stop");
        initCtl("disable");
        QFile::remove(serviceFilePath());
        initCtlReload();
    }

    emit serviceEnabledChanged();
}
