cd /third/
git clone https://github.com/screeninvader/xf86-video-fbturbo.git
cd xf86-video-fbturbo
./autogen.sh
./configure
make clean
make
make install

