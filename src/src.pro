DEFINES += QWEBDAVITEM_EXTENDED_PROPERTIES
QMAKE_CXXFLAGS += -std=gnu++0x

TEMPLATE = subdirs
SUBDIRS = app daemon permission-agent qmlextension sharing
