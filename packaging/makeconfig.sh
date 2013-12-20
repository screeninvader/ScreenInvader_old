rm -r screeninvader-config/etc
cp -a ../src/etc screeninvader-config/
dpkg-deb -b screeninvader-config/ screeninvader-config_0.9-all.deb

