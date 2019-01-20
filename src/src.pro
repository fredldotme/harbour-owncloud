QMAKE_CXXFLAGS += -std=gnu++0x

TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS = common app

!contains(CONFIG, noadditionals) {
    SUBDIRS += daemon permission-agent qmlextension sharing
}
