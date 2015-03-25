cd /third
git clone https://github.com/screeninvader/libump.git
cd libump
autoreconf -i
./configure
make clean
make
make install

