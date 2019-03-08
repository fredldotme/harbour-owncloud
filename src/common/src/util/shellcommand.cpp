#include "shellcommand.h"

#include <QDebug>

void ShellCommand::runCommand(QString command, QStringList args)
{
#ifndef QT_NO_PROCESS
    QProcess *proc = new QProcess();
    proc->setReadChannel(QProcess::StandardOutput);
    connect(proc, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), proc, &QProcess::deleteLater, Qt::DirectConnection);
    proc->start(command, args);
#else
    qWarning() << "QProcess not supported on the target platform";
#endif
}
