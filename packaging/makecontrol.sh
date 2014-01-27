#!/bin/bash

if [ $# -ne 2 ]; then
  echo "Usage: makecontrol.sh <component> <version>"
fi


cat <<EOCONTROL
Package: screeninvader-$1
Version: $2
Architecture: armhf
Maintainer: Amir Hassan <amir@viel-zu.org>
Installed-Size: 1099
Section: misc
Priority: optional
Homepage: http://screeninvader.com
Description: ScreenInvader $1 files
EOCONTROL

