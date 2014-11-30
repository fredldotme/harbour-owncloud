#include "shellcommand.h"

ShellCommand::ShellCommand(QObject *parent) : QObject(parent)
{
}

void ShellCommand::runCommand(QString command, QStringList args)
{
    QProcess *proc = new QProcess();
    proc->setReadChannel(QProcess::StandardOutput);
    connect(proc, SIGNAL(finished(int)), proc, SLOT(terminate()));
    proc->start(command, args);
}
