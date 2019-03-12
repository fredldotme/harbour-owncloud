TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS = 3rdparty/qwebdavlib/qwebdavlib src

android {
    !exists($$OUT_PWD/3rdparty/build_android_openssl.sh): \
        error(The Android OpenSSL build script is missing, please copy it\
                to '$$OUT_PWD/3rdparty/build_android_openssl.sh'.)

    exists($$OUT_PWD/3rdparty/build_android_openssl.sh) {
        !exists ($$OUT_PWD/3rdparty/openssl) {
            warning(OpenSSL library missing)
            error(Change to the $$OUT_PWD/3rdparty directory \
                and run the script manually)
            system($$OUT_PWD/3rdparty/build_android_openssl.sh --help)
        }
    }
}

OTHER_FILES += \
    rpm/harbour-owncloud.changes.in \
    rpm/harbour-owncloud.spec \
    rpm/harbour-owncloud.yaml
