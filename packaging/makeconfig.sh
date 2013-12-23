#!/bin/bash

if [ $# -ne 1 ]; then
  echo "Usage: makeconfig.sh <version>"
fi

./makecontrol.sh config $1 > screeninvader-config/DEBIAN/control
rm -r screeninvader-config/etc
cp -a ../src/etc screeninvader-config/
dpkg-deb -b screeninvader-config/ screeninvader-config-all.deb
dpkg-sig --sign builder screeninvader-config-all.deb
