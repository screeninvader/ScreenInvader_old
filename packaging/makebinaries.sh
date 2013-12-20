rm -r screeninvader-binaries/lounge
mkdir -p screeninvader-binaries/lounge/bin
cp -a ../build/* screeninvader-binaries/lounge/bin/
dpkg-deb -b screeninvader-binaries/ screeninvader-binaries_0.9-all.deb

