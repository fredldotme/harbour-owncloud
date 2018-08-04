QMAKE_CXXFLAGS += -std=gnu++0x

TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS = common app daemon permission-agent qmlextension sharing
