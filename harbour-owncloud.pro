TEMPLATE = subdirs
SUBDIRS = qwebdavlib/qwebdavlib app

QMAKE_CXXFLAGS += -std=gnu++0x

OTHER_FILES += rpm
