#!/bin/sh

export LD_LIBRARY_PATH=$PWD/usr/lib:$LD_LIBRARY_PATH
export QT_QUICK_CONTROLS_MOBILE=true
export QT_QUICK_CONTROLS_STYLE=Material
export QT_QUICK_CONTROLS_MATERIAL_THEME=Light
export QT_QUICK_CONTROLS_MATERIAL_PRIMARY=white
export QT_QUICK_CONTROLS_MATERIAL_FOREGROUND=black

exec $PWD/usr/bin/harbour-owncloud \
    --desktop_file_hint=${HOME}/.local/share/applications/${APP_ID}.desktop "$@"
