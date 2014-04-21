#!/bin/bash

if [ $# -ne 1 ]; then
  echo "Usage: makecore.sh <version>"
fi

./makecontrol.sh core $1 > screeninvader-core/DEBIAN/control
rm -r screeninvader-core/root screeninvader-core/lounge screeninvader-core/setup
cp -a ../src/setup ../src/lounge ../src/root screeninvader-core/
chown -r root:root screeninvader-core/root
chown -r 1000:1000 screeninvader-core/lounge
dpkg-deb -b screeninvader-core/ screeninvader-core-all.deb
dpkg-sig --sign builder screeninvader-core-all.deb
