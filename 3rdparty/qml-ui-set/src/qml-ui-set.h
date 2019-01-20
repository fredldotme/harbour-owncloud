#ifndef QMLUISET_H
#define QMLUISET_H

#include <QtQml>

#include "localfilebrowser.h"
#include "filedetailshelper.h"

namespace QmlUiSet {
void registerQmlTypes() {
    qmlRegisterType<LocalFileBrowser>("QmlUiSet", 1, 0, "LocalFileBrowser");
    qmlRegisterType<FileDetailsHelper>("QmlUiSet", 1, 0, "FileDetailsHelper");
}
}

#endif // QMLUISET_H
