#!/bin/bash

if [ "$1" == "-h" ] || [ "$1" == "--help" ]; then
    echo "Usage: $0 <openssl-version> <android-target-arch>"
    echo ""
    echo "Make sure to have the ANDROID_NDK_HOME environment variable pointing"
    echo "to your Android NDK installation."
    echo ""
    echo "Examples:"
    echo "For arm: $0 1.0.2q arm"
    echo "For arm64: $0 1.0.2q arm64"
    exit 0
fi

if [ "$1" == "" ]; then
    echo "OpenSSL version not specified. Bailing..."
    exit 1
fi

if [ "$2" == "" ]; then
    echo "Android target architecture not specified. Bailing..."
    exit 2
fi

if [ "$ANDROID_NDK_HOME" == "" ]; then
    echo "ANDROID_NDK_HOME not specified"
    exit 3
fi

OPENSSL_VERSION=$1
ANDROID_TARGET_ARCH=$2
TOOLCHAIN_INSTALL_DIR="${ANDROID_NDK_HOME}/generated_toolchains"
ANDROID_TARGET_API=21
OPENSSL_DIR=openssl-${OPENSSL_VERSION}
OPENSSL_FILE=${OPENSSL_DIR}.tar.gz

if [ "$ANDROID_TARGET_ARCH" == "arm" ]; then
    ANDROID_BUILD_ARCH="arm"
    ABI=androideabi
elif [ "$ANDROID_TARGET_ARCH" == "arm64" ]; then
    ANDROID_BUILD_ARCH="aarch64"
    ABI=android
elif [ "$ANDROID_TARGET_ARCH" == "x86" ]; then
    ANDROID_BUILD_ARCH="i686"
    ABI=android
elif [ "$ANDROID_TARGET_ARCH" == "x86_64" ]; then
    ANDROID_BUILD_ARCH="x86_64"
    ABI=android
else
    echo "Failed to determine ANDROID_BUILD_ARCH. Bailing..."
    exit 4
fi

TOOLCHAIN_FULL_PATH=${TOOLCHAIN_INSTALL_DIR}/android-${ANDROID_TARGET_API}-${ANDROID_TARGET_ARCH}

# Toolchain preparation
$ANDROID_NDK_HOME/build/tools/make_standalone_toolchain.py \
    --arch ${ANDROID_TARGET_ARCH} \
    --api ${ANDROID_TARGET_API} \
    --install-dir ${TOOLCHAIN_FULL_PATH}

# Fetch and unpack
if [ ! -f ${OPENSSL_FILE} ]; then
    wget https://www.openssl.org/source/${OPENSSL_FILE}
fi
tar xvf ${OPENSSL_FILE}
cd ${OPENSSL_DIR}

# Building
find . -type f -print0 | xargs -0 sed -i  's/-mandroid//g'

CROSS_COMPILE=${TOOLCHAIN_FULL_PATH}/bin/${ANDROID_BUILD_ARCH}-linux-${ABI}- ./Configure android shared
make ANDROID_DEV=${TOOLCHAIN_FULL_PATH}/sysroot/usr CALC_VERSIONS="SHLIB_COMPAT=; SHLIB_SOVER=" build_libs

if [ ! -f "libcrypto.so" ]; then
    echo "libcrypto.so doesn't exist. Bailing..."
    exit 5
fi

if [ ! -f "libssl.so" ]; then
    echo "libssl.so doesn't exist. Bailing..."
    exit 6
fi

cd ..

if [ -d openssl ]; then
    rm -r openssl
fi

mv ${OPENSSL_DIR} openssl

echo "Build successful!"
