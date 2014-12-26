DEFINES += QWEBDAVITEM_EXTENDED_PROPERTIES
QMAKE_CXXFLAGS += -std=gnu++0x

TEMPLATE = subdirs
SUBDIRS = qwebdavlib/qwebdavlib app daemon

OTHER_FILES += rpm
