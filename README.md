ScreenInvader - A content centered media center 
=========================================

#### Install Image #####

Download the image from http://tbd/screeninvader.lzma and write it to your an external hard disk.

    wget -O - http://tbd/screeninvader.lzma > lzcat /dev/sdX

#### Build the ScreenInvader system from scratch ####

##### Prepare an external boot device ######

    # makestick.sh prepares an external boot device to install
    cd LoungeMC/
    ./makestick.sh /dev/sdX 400

    # Mount the boot device
    mount /dev/sdX /media/stick

    # Run bootstrap.sh to create the file system.
    ./bootstrap.sh /media/stick

-------

GPL V2

Contact
-------

Amir Hassan: <amir@viel-zu.org>

