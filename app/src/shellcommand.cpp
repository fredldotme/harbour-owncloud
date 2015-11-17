#include "shellcommand.h"

ShellCommand::ShellCommand(QObject *parent) : QObject(parent)
{
}

void ShellCommand::runCommand(QString command, QStringList args)
{
    QProcess *proc = new QProcess();
    proc->setReadChannel(QProcess::StandardOutput);
    connect(proc, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), proc, &QProcess::deleteLater, Qt::DirectConnection);
    proc->start(command, args);
}
