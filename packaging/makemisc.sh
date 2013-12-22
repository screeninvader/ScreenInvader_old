#!/bin/bash

if [ $# -ne 1 ]; then
  echo "Usage: makemisc.sh <version>"
fi

./makecontrol.sh misc $1 > screeninvader-misc/DEBIAN/control
rm -r screeninvader-misc/usr
cp -a ../src/usr screeninvader-misc/
dpkg-deb -b screeninvader-misc/ screeninvader-misc-all.deb

