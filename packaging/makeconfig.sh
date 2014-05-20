#!/bin/bash

if [ $# -ne 1 ]; then
  echo "Usage: makeconfig.sh <version>"
fi

./makecontrol.sh config $1 > screeninvader-config/DEBIAN/control
rm -rf screeninvader-config/etc
cp -a ../src/etc screeninvader-config/
chown root:root screeninvader-config/etc/sudoers 
chown root:root screeninvader-config/etc/passwd
chmod 0440 screeninvader-config/etc/sudoers
dpkg-deb -b screeninvader-config/ screeninvader-config-all.deb
dpkg-sig --sign builder screeninvader-config-all.deb
