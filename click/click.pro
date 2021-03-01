# This creates the manifest.json file, it is the description file for the
# click package.

TEMPLATE = aux

# figure out the current build architecture
CLICK_ARCH=$$system(dpkg-architecture -qDEB_HOST_ARCH)

# do not remove this line, it is required by the IDE even if you do
# not substitute variables in the manifest file
UBUNTU_MANIFEST_FILE = $$PWD/manifest.json.in


# substitute the architecture in the manifest file
manifest_file.output   = manifest.json
manifest_file.CONFIG  += no_link \
                         add_inputs_as_makefile_deps\
                         target_predeps
manifest_file.commands = sed s/@CLICK_ARCH@/$$CLICK_ARCH/g ${QMAKE_FILE_NAME} > ${QMAKE_FILE_OUT}
manifest_file.input = UBUNTU_MANIFEST_FILE
QMAKE_EXTRA_COMPILERS += manifest_file

# installation path of the manifest file
mfile.CONFIG += no_check_exist
mfile.files  += $$OUT_PWD/manifest.json
mfile.path = /

# AppArmor profile
apparmor.files += $$PWD/harbour-owncloud.apparmor
apparmor.path = /

# Daemon AppArmor profile
daemonapparmor.files += $$PWD/harbour-owncloud-daemon.apparmor
daemonapparmor.path = /

# Desktop launcher icon
desktopicon.files += $$PWD/ghostcloud.png
desktopicon.path = /

# Desktop launcher
desktop.files += $$PWD/harbour-owncloud.desktop
desktop.path = /

# Accounts description
accounts.files += $$PWD/harbour-owncloud.accounts
accounts.path = /

# Daemon desktop launcher
daemondesktop.files += $$PWD/harbour-owncloud-daemon.desktop
daemondesktop.path = /

# Daemon launcher
daemon.files += $$PWD/harbour-owncloud-daemon.json
daemon.path = /

# Run script
runscript.files += $$PWD/run.sh
runscript.path = /

# Daemon run script
rundaemonscript.files += $$PWD/run-daemon.sh
rundaemonscript.path = /

INSTALLS += mfile apparmor daemonapparmor runscript rundaemonscript desktopicon desktop daemondesktop daemon accounts
