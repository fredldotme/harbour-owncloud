#include "shellcommand.h"

ShellCommand::ShellCommand(QObject *parent) : QObject(parent)
{
}

void ShellCommand::runCommand(QString command, QStringList args)
{
    QProcess *proc = new QProcess();
    proc->setReadChannel(QProcess::StandardOutput);
    connect(proc, &QProcess::finished, proc, &QObject::deleteLater);
    proc->start(command, args);
}
