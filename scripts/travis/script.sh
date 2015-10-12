#!/bin/bash
set -e

qmake -v
packthing -h

case "$PLATFORM" in
"osx")
    packthing -j4 dmg
    mv build/staging/propelleride-*.dmg .
    ;;
"linux")
    packthing -j4 run
    mv build/staging/propelleride-*.run .

    fakeroot packthing -j4 deb
    mv build/staging/propelleride-*.deb .
    ;;
"rpi")
    packthing rpi --checkout-only

    pushd $RPISCRIPTS
    sudo chroot $MNT/ /bin/bash -c "./inside-chroot-script.sh"
    popd
    ;;
*)
    echo "Invalid PLATFORM"
    exit 1
    ;;
esac
