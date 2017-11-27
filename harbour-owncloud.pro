DEFINES += QWEBDAVITEM_EXTENDED_PROPERTIES
QMAKE_CXXFLAGS += -std=gnu++0x

TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS = qwebdavlib/qwebdavlib app daemon sharing

OTHER_FILES += rpm
