#define DEBUG_WEBDAV 1
#include "filesystem.h"
#include "uploader.h"
#include "settings.h"

#include <QCoreApplication>

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    Settings::instance()->readSettings();

    Filesystem fsHandler;
    Uploader uploader;
    QObject::connect(&fsHandler, SIGNAL(fileFound(QString)), &uploader, SLOT(fileFound(QString)));
    QObject::connect(&uploader, SIGNAL(localPathUpdated()), &fsHandler, SLOT(localPathChanged()));

    return app.exec();
}
