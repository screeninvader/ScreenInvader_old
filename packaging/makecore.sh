rm -r screeninvader-core/root screeninvader-core/lounge screeninvader-core/setup
cp -a ../src/setup ../src/lounge ../src/root screeninvader-core/
dpkg-deb -b screeninvader-core/ screeninvader-core_0.9-all.deb

