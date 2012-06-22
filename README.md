LoungeMC - A content centered media center 
=========================================

#### Install Image #####

Download the image from http://tbd/LoungeMC.lzma and write it to your an external hard disk.

    wget -O - http://tbd/LoungeMC.lzma > lzcat /dev/sdX

#### Build the lounge mc system from scratch ####

##### Prepare an external boot device ######

		# makestick.sh prepares an external boot device to install
    cd LoungeMC/
    ./makestick.sh /dev/sdX 400

		# Run bootstrap.sh to create the LoungeMC file system.
    ./bootstrap.sh 

-------

GPL V2

Contact
-------

Amir Hassan: <amir@viel-zu.org>

