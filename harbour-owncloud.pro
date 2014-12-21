DEFINES += QWEBDAVITEM_EXTENDED_PROPERTIES
QMAKE_CXXFLAGS += -std=gnu++0x

include(notifications.pri)

TEMPLATE = subdirs
SUBDIRS = qwebdavlib/qwebdavlib app

OTHER_FILES += rpm
