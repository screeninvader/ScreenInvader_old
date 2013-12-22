#!/bin/bash

if [ $# -ne 1 ]; then
  echo "Usage: makecore.sh <version>"
fi

./makecontrol.sh core $1 > screeninvader-core/DEBIAN/control
rm -r screeninvader-core/root screeninvader-core/lounge screeninvader-core/setup
cp -a ../src/setup ../src/lounge ../src/root screeninvader-core/
dpkg-deb -b screeninvader-core/ screeninvader-core-all.deb

