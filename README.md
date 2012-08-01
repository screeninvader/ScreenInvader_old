ScreenInvader - A content centered media center 
=========================================

#### Install Image #####

Download the image from http://tbd/screeninvader.dd.xz and write it to an external storage device.
    # as root user
    wget -O - https://github.com/downloads/kallaballa/ScreenInvader/screeninvader.dd.xz > xzcat /dev/sdX

#### Build the ScreenInvader system from scratch ####

##### Prepare an external boot device ######

    # makestick.sh prepares an external boot device to install
    cd ScreenInvader/
    ./makestick.sh /dev/sdX 500

    # Mount the created partition
    mount /dev/sdX1 /media/stick

    # Run bootstrap.sh to create the file system.
    ./bootstrap.sh /media/stick

-------

GPL V2

Contact
-------

Amir Hassan: <amir@viel-zu.org>

