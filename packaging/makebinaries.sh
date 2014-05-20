#!/bin/bash

if [ $# -ne 1 ]; then
  echo "Usage: makebinaries.sh <version>"
fi

./makecontrol.sh binaries $1 > screeninvader-binaries/DEBIAN/control
rm -fr screeninvader-binaries/lounge
mkdir -p screeninvader-binaries/lounge/bin
cp -a ../build/* screeninvader-binaries/lounge/bin/
dpkg-deb -b screeninvader-binaries/ screeninvader-binaries-all.deb
dpkg-sig --sign builder screeninvader-binaries-all.deb
