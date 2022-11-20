QMAKE_CXXFLAGS += -std=gnu++0x

TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS = common

!contains(CONFIG, noapp) {
    SUBDIRS += app
}

contains(CONFIG, noadditionals) {
    CONFIG += nodaemon nopermissionagent noqmlextension nosharing
}

!contains(CONFIG, nodaemon) {
    SUBDIRS += daemon
}

!contains(CONFIG, nopermissionagent) {
    SUBDIRS += permission-agent
}

!contains(CONFIG, noqmlextension) {
    SUBDIRS += qmlextension
}

!contains(CONFIG, nosharing) {
    SUBDIRS += sharing
}

contains(CONFIG, clickphotobackup) {
    SUBDIRS += ut-photobackup
}
