DEFINES += QWEBDAVITEM_EXTENDED_PROPERTIES
QMAKE_CXXFLAGS += -std=gnu++0x

TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS = 3rdparty/qwebdavlib/qwebdavlib src

OTHER_FILES += \
    rpm/harbour-owncloud.changes.in \
    rpm/harbour-owncloud.spec \
    rpm/harbour-owncloud.yaml
