rm -r screeninvader-misc/usr
cp -a ../src/usr screeninvader-misc/
dpkg-deb -b screeninvader-misc/ screeninvader-misc_0.9-all.deb

