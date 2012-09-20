#!/bin/bash
#
# ScreenInvader - A shared media experience. Instant and seamless.
#  Copyright (C) 2012 Amir Hassan <amir@viel-zu.org>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#

function getConf() {
  cat "$1" | tr "\n" " "
}

KERNEL="`getConf config/kernel`"
VIDEO_DRIVERS="`getConf config/video_drivers`"
KEYRINGS="`getConf config/keyrings`"
PKG_WHITE="`getConf config/packages_white`"
PKG_EXTRA="`getConf config/packages_extra`"
PKG_BLACK="`getConf config/packages_black`" 
PKG_SID="`getConf config/packages_sid`"
FILES_BLACK="`getConf config/files_black`"

export LC_ALL="C"

APTNI="apt-get -q -y --no-install-recommends --force-yes -o Dpkg::Options::=\"--force-confdef\" -o Dpkg::Options::=\"--force-confold\" ";

DEBIAN_MIRROR="http://ftp.at.debian.org/debian/"
EMDEBIAN_MIRROR="http://www.emdebian.org/grip/"
DEBIAN_MULTIMEDIA_MIRROR="http://www.deb-multimedia.org/"

dir="`dirname $0`"
BOOTSTRAP_DIR="`cd $dir; pwd`"
BOOTSTRAP_LOG="$BOOTSTRAP_DIR/bootstrap.log"
ARCH=i386
APTCACHER_PORT=
NOINSTALL=
NODEBOOT=
CHROOT_DIR=
CHRT=
DEBUG=
GIDX=

function printUsage() {
  cat 0>&2 <<EOUSAGE
Bootstrap a ScreenInvader file system.

$0 [-a <arch>][-g <num>][-l <logfile>][-p <apt-cacher-port>][-i -d -u -x] <bootstrapdir>
Options:
  -a <arch> Bootstrap a system of the given architecture
  -g <num>  Build with selected graphics card
  -l <file> Specify the log file
  -p <port> Enables using apt-cacher-ng on the specified port
  -i        Don't configure and install packages
  -d        Don't debootstrap
  -u        Combined -d and -i
  -x        Install extra packages for debugging
EOUSAGE
  exit 1
}

function absDir() {
  dir="`dirname $1`"
  absdir="`cd $dir; pwd`"
  echo $absdir
}

function absPath() {
  dir="`dirname $1`"
  base="`basename $1`"
  absdir="`cd $dir; pwd`"
  echo $absdir/$base
}

function printVideoDrivers() {
  PAD=18
  i=0

  echo $VIDEO_DRIVERS | sed 's/ /\n/g' | cut -d"-" -f4| while read vd; do
    LEN=$[ ${#i} + ${#vd} + 2 ]
    echo -n "($i) $vd"
  
    for j in `seq 0 $[$PAD - $LEN]`; do echo -n " "; done
    i=$[$i + 1]
    if [ $[ $i % 3 ] -eq 0 ]; then
      echo
    fi
  done  
  echo
}

function askVideoDriver() {
  NUM="`echo $VIDEO_DRIVERS | wc -w`"
  echo -n "Please select a video driver (default=0):" 1>&2
  DRIVER=""
  while read idx; do
    [ -z "$idx" ] && idx=0
    if printf "%d" $idx > /dev/null 2>&1; then
      if [ $idx -lt 0 -o $idx -ge $NUM ]; then
	echo "Out of range: $idx." 1>&2
      else
	DRIVER="`echo $VIDEO_DRIVERS | sed 's/ /\n/g' | sed -n "$[ $idx + 1 ]p"`"
	echo "Selected: $DRIVER" 1>&2
	break
      fi
    else
      echo "Invalid input: $idx. Please select the video driver by entering a number." 1>&2
    fi
    echo -n "Please select a video driver (default=0):" 1>&2
  done
  echo $DRIVER
}

function skip() {
  echo -n "$1: "
  green "skipped\n"
}

function doDebootstrap() {
  check "Create target dir" \
    "mkdir -p \"$CHROOT_DIR\""

  BOOTSTRAP_MIRROR=$DEBIAN_MIRROR

  [ -n "$APTCACHER_PORT" ] && BOOTSTRAP_MIRROR=$(
    HOST="`echo $BOOTSTRAP_MIRROR | sed 's/^http*:\/\///g' | sed 's/\/.*$//g'`"
    echo "http://127.0.0.1:$APTCACHER_PORT/$HOST/debian"
  )
 check "Bootstrap debian" \
    "debootstrap --exclude="`echo $PKG_BLACK | sed 's/ /,/g'`" --arch $ARCH squeeze "$CHROOT_DIR" $BOOTSTRAP_MIRROR"
}

function doPackageConf() {
  export DEBIAN_FRONTEND=noninteractive

  check "Prune debconf cache" \
    "$CHRT mkdir -p /var/cache/debconf/"

  check "Prepare package manager" \
    "$CHRT dpkg --configure -a"

  check "Fix dependencies" \
    "$CHRT $APTNI install -f"

  check "Update Repositories" \
    "$CHRT $APTNI update"

  check "Install keyrings" \
    "$CHRT $APTNI install $KEYRINGS"

  check "Update Repositories" \
    "$CHRT $APTNI update"

  check "Update apt policy" \
    "$CHRT bash -c 'touch /var/lib/apt/lists/*; apt-cache policy'"

  check "Install white packages" \
    "$CHRT $APTNI install $PKG_WHITE"

  check "Install sid packages" \
     "$CHRT $APTNI -t sid-grip install $PKG_SID"

  check "Install kernel" \
    "$CHRT $APTNI -t squeeze install $KERNEL"

  check "Upgrade packages" \
    "$CHRT $APTNI upgrade"

  check "Remove black listed packages" \
    "$CHRT $APTNI purge $PKG_BLACK"
}

function doCopy() {
  check "Copy system changes" \
    "cd $BOOTSTRAP_DIR/src; rsync -axh etc usr $CHROOT_DIR/"
  
  check "Sync lounge changes" \
    "cd $BOOTSTRAP_DIR/src; rsync -axh --delete lounge $CHROOT_DIR/"

  check "Sync root changes" \
    "cd $BOOTSTRAP_DIR/src; rsync -axh --delete root $CHROOT_DIR/"

  check "Sync setup changes" \
    "cd $BOOTSTRAP_DIR/; rsync -axh --delete setup $CHROOT_DIR/"

  check "Copy plymouth theme" \
    "cp -a $BOOTSTRAP_DIR/themes/screeninvader $CHROOT_DIR/usr/share/plymouth/themes/"

  check "Copy xosd lib" \
    "cp -a $BOOTSTRAP_DIR/build/xosd-2.2.14/src/libxosd/.libs/libxosd.so.2.2.14 $CHROOT_DIR/usr/lib/"

  check "ldconfig" \
    "$CHRT ldconfig"

  check "Copy osd binary"  \
    "cp -a $BOOTSTRAP_DIR/build/xosd-2.2.14/src/screeninvader_plugin/osd $CHROOT_DIR/lounge/bin/"

  check "Copy janosh binary"  \
    "cp -a $BOOTSTRAP_DIR/build/janosh $CHROOT_DIR/lounge/bin/"

  check "Update plymouth theme" \
    "$CHRT plymouth-set-default-theme -R screeninvader"

}

function doCleanupPackages() {
  check "Autoremove packages" \
    "$CHRT $APTNI autoremove"

  check "Clean apt cache" \
    "$CHRT $APTNI clean"
}

function doCleanupFiles() {
  check "Remove black listed files" \
              "$CHRT bash -c \"rm -fr $FILES_BLACK\""
}

function doPrepareChroot() {
  cd "$CHROOT_DIR"
  check "Bind chroot dev fs" \
    "mount --bind /dev/ dev"
  check "Create chroot procs fs" \
     "mount -t proc none proc"
  check "Create chroot sys fs" \
      "mount -t sysfs none sys"
  check "Create chroot tmpfs fs" \
      "mount -t tmpfs none tmp"
  check "Create chroot devpts fs" \
      "mount -t devpts none dev/pts"

  check "Prune apt directories" \
      "mkdir -p \"$CHROOT_DIR/etc/apt/\" \"$CHROOT_DIR/etc/apt/preferences.d/\" \"$CHROOT_DIR/etc/apt/apt.conf.d/\""

  check "Make apt preferences" \
    "\"$BOOTSTRAP_DIR/templates/apt_preferences\" > \"$CHROOT_DIR/etc/apt/preferences.d/prefere_em_squeeze\""

  check "Make apt sources list" \
    "\"$BOOTSTRAP_DIR/templates/sources_list\" \"$EMDEBIAN_MIRROR\" \"$DEBIAN_MIRROR\" \"$DEBIAN_MULTIMEDIA_MIRROR\" > \"$CHROOT_DIR/etc/apt/sources.list\""

  if [ -n "$APTCACHER_PORT" ]; then
    # use apt-cacher-ng to cache packages during install
    check "Make apt cacher conf" \
      "\"$BOOTSTRAP_DIR/templates/00aptcacher\" \"$APTCACHER_PORT\" > \"$CHROOT_DIR/etc/apt/apt.conf.d/00aptcacher\""
  fi

  # disable starting daemons after install
  check "Prune /usr/sbin" \
      "mkdir -p \"$CHROOT_DIR/usr/sbin\""

  check "Make policy-rd.d" \
    "$BOOTSTRAP_DIR/templates/policy-rc_d > $CHROOT_DIR/usr/sbin/policy-rc.d"

  check "Fix policy-rd.d permissions" \
      "chmod 755 \"$CHROOT_DIR/usr/sbin/policy-rc.d\""
}

function doFreeChroot() {
  pkill -KILL -P $$ &> /dev/null
  ( 
    cd "$CHROOT_DIR"
    umount dev/pts
    umount tmp
    umount sys 
    umount proc
    umount dev
    umount -l dev
  ) &>/dev/null
  rm -rf "$CHROOT_DIR/etc/apt/apt.conf.d/00aptcacher"
  rm -rf "$CHROOT_DIR/usr/sbin/policy-rc.d"
  exit
}

function doCheckPreCond() {
  check "'debootstrap' installed" \
    "which debootstrap"
}

function doCreateBuildHtml() {
  cat > $CHROOT_DIR/lounge/www/build.html <<EOHTML
<html>
<head>
<title>ScreenInvader</title>
</head>
<body style="background-color: #000000">
<h3 style="padding: 20px; color: #ff6600;">You are using ScreenInvader version</h3>
<table>
<tr>
<td colspan="2" style="padding: 20px; width:100%; color: #ffffff; font-variant:bold;">
`date`
</td>
</tr>
</table>
</body>
</html>
EOHTML
}

###### main

while getopts 'a:l:p:g:idux' c
do
  case $c in
    a) ARCH="$OPTARG";;
    l) BOOTSTRAP_LOG="`absPath $OPTARG`";;
    p) APTCACHER_PORT="$OPTARG";;
    i) NOINSTALL="YES";;
    g) GIDX="$OPTARG";;
    d) NODEBOOT="YES";;
    u) NOINSTALL="YES"; NODEBOOT="YES";;
    x) INSTALL_EXTRA="YES";;
    \?) printUsage;;
  esac
done

shift $(($OPTIND - 1))

echo > "$BOOTSTRAP_LOG"
export BOOTSTRAP_LOG
source "$BOOTSTRAP_DIR/.functions.sh"

if [ $# -ne 1 ]; then
  printUsage
else
  export CHROOT_DIR="`absPath $1`"
  export CHRT="chroot \"$CHROOT_DIR\" "

  doCheckPreCond

  printVideoDrivers
  
  if [ -z "$GIDX" ]; then
    PKG_WHITE="$PKG_WHITE $(askVideoDriver)"
  else
    DRIVER="`echo $VIDEO_DRIVERS | sed 's/ /\n/g' | sed -n "$[ $GIDX + 1 ]p"`"
    PKG_WHITE="$PKG_WHITE $DRIVER"
  fi
  [ -n "$INSTALL_EXTRA" ] && PKG_WHITE="$PKG_WHITE $PKG_EXTRA" 

  if [ -z "$NODEBOOT" ]; then 
    doDebootstrap
  else
    skip "debootstrap"
  fi

  doPrepareChroot
  # make sure we're cleaning up eventually
  trap doFreeChroot SIGINT SIGTERM EXIT

  if [ -z "$NOINSTALL" ]; then 
    doPackageConf
  else
    skip "package configuration"
  fi

  doCleanupPackages
  doCleanupFiles
  doCopy
  doCreateBuildHtml
fi


exit 0

