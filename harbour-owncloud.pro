TEMPLATE = subdirs
CONFIG += ordered

# Use shared libraries and dependencies on every platform
!macx {
    SUBDIRS = 3rdparty/qwebdavlib/qwebdavlib src
}
# .. except macOS
macx {
    SUBDIRS = src
}

# Android OpenSSL script requirement
android {
    !exists($$OUT_PWD/3rdparty/build_android_openssl.sh): \
        error(The Android OpenSSL build script is missing, please copy it\
                to '$$OUT_PWD/3rdparty/build_android_openssl.sh'.)

    exists($$OUT_PWD/3rdparty/build_android_openssl.sh) {
        !exists ($$OUT_PWD/3rdparty/openssl) {
            warning(OpenSSL library missing)
            system($$OUT_PWD/3rdparty/build_android_openssl.sh --help)
            error(Change to the $$OUT_PWD/3rdparty directory \
                and run the script manually)
        }
    }
}

# Ubuntu Touch click packaging
contains(CONFIG,click) {
    SUBDIRS += click
}

# SailfishOS rpm packaging
OTHER_FILES += \
    rpm/harbour-owncloud.changes.in \
    rpm/harbour-owncloud.spec \
    rpm/harbour-owncloud.yaml
