DEFINES += QWEBDAVITEM_EXTENDED_PROPERTIES
QMAKE_CXXFLAGS += -std=gnu++0x

TEMPLATE = subdirs
SUBDIRS = qwebdavlib/qwebdavlib app

OTHER_FILES += rpm
