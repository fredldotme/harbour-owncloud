#include "dummyintentfileselector.h"

#include <QDebug>

DummyIntentFileSelector::DummyIntentFileSelector(QObject *parent) : QObject(parent)
{

}

void DummyIntentFileSelector::fileSelectIntent()
{
    qInfo() << "NOT IMPLEMENTED";
}
