#!/bin/bash
#
# LoungeMC - A content centered media center 
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

KERNEL="linux-image-686"

VIDEO_DRIVERS="xserver-xorg-video-all xserver-xorg-video-ati xserver-xorg-video-radeon xserver-xorg-video-nv xserver-xorg-video-intel xserver-xorg-video-geode xserver-xorg-video-glide xserver-xorg-video-glint xserver-xorg-video-i128 xserver-xorg-video-i740 xserver-xorg-video-mach64 xserver-xorg-video-geode xserver-xorg-video-cirrus xserver-xorg-video-mga xserver-xorg-video-openchrome xserver-xorg-video-via xserver-xorg-video-fbdev xserver-xorg-video-dummy xserver-xorg-video-glamo xserver-xorg-video-apm  xserver-xorg-video-ark  xserver-xorg-video-chips xserver-xorg-video-neomagic xserver-xorg-video-nouveau xserver-xorg-video-qxl  xserver-xorg-video-r128 xserver-xorg-video-radeonhd xserver-xorg-video-rendition xserver-xorg-video-s3 xserver-xorg-video-s3virge xserver-xorg-video-savage xserver-xorg-video-siliconmotion xserver-xorg-video-sis  xserver-xorg-video-sisusb xserver-xorg-video-tdfx xserver-xorg-video-tga xserver-xorg-video-trident xserver-xorg-video-tseng xserver-xorg-video-vesa xserver-xorg-video-vmware xserver-xorg-video-voodoo"

PKG_WHITE="debian-multimedia-keyring keyboard-configuration debconf-english sudo dialog mplayer-nogui thttpd feh mpd mpc xdotool alsa-utils awesome psmisc clive midori dos2unix curl dropbear xinit autofs smbfs mingetty xserver-xorg xserver-xorg-input-kbd xserver-xorg-input-mouse x11-xserver-utils locate plymouth xfonts-intl-european gifsicle kbd"

PKG_EXTRA="rsyslogd vim rsync less"

PKG_BLACK="info manpages rsyslog tasksel tasksel-data aptitude locales man-db whiptail iptables wmctrl vim-tiny vim-common traceroute netcat-traditional iputils-ping dmidecode libboost-iostreams1.42.0 libcwidget3 libept1 libnewt0.52 libnfnetlink0 libsigc++-2.0-0c2a"

FILES_BLACK="/var/cache/apt/pkgcache.bin /var/cache/apt/srcpkgcache.bin /usr/share/man/* /usr/share/locale/* /usr/share/doc/* /usr/share/zoneinfo/* /usr/share/icons/* /root/.bash_history /lib/modules/*/kernel/drivers/infiniband/* /lib/modules/*/kernel/drivers/bluetooth/* /lib/modules/*/kernel/drivers/media/* /lib/modules/*/kernel/drivers/net/wireless/* /var/cache/debconf* /usr/share/doc-base/*"

export LC_ALL="C"
DEBIAN_MIRROR="http://ftp.at.debian.org/debian/"
EMDEBIAN_MIRROR="http://ftp.at.debian.org/debian/"
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
  cat 1>&2 <<EOUSAGE
Bootstrap a Lounge Media Center installation.

$0 [-a <arch>][-l <logfile>][-c <apt-cacher-port>][-i -d -u ] <bootstrapdir>"
Options:"
  -a <arch> Bootstrap a system with of the given architecture
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

  BOOTSTRAP_MIRROR=$EMDEBIAN_MIRROR

  [ -n "$APTCACHER_PORT" ] && BOOTSTRAP_MIRROR=$(
    HOST="`echo $EMDEBIAN_MIRROR | sed 's/^http*:\/\///g' | sed 's/\/.*$//g'`"
    echo "http://127.0.0.1:$APTCACHER_PORT/$HOST/debian"
  )

  check "Bootstrap debian" \
    "debootstrap --arch $ARCH squeeze "$CHROOT_DIR" $BOOTSTRAP_MIRROR"
}

function doPackageConf() {
  export DEBIAN_FRONTEND=noninteractive
  aptni="apt-get -q -y --no-install-recommends --force-yes -o Dpkg::Options::=\"--force-confdef\" -o Dpkg::Options::=\"--force-confold\" ";

  check "Prune debconf cache" \
    "$CHRT mkdir -p /var/cache/debconf/"

  check "Prepare package manager" \
    "$CHRT dpkg --configure -a"

  check "Fix dependencies" \
    "$CHRT $aptni install -f"

  check "Update Repositories" \
    "$CHRT $aptni update"

  check "Install white listed packages" \
    "$CHRT $aptni install $PKG_WHITE"

  check "Install kernel" \
    "$CHRT $aptni install $KERNEL"

  check "Remove black listed packages" \
    "$CHRT $aptni purge $PKG_BLACK"

  check "Autoremove packages" \
    "$CHRT $aptni autoremove"
}

function doCopy() {
  check "Copy system data" \
    "cd $BOOTSTRAP_DIR/data; rsync -axh etc usr $CHROOT_DIR/"
  
  check "Sync lounge data" \
    "cd $BOOTSTRAP_DIR/data; rsync -axh --delete lounge $CHROOT_DIR/"

  check "Sync setup data" \
    "cd $BOOTSTRAP_DIR/; rsync -axh --delete setup $CHROOT_DIR/"

  check "Copy plymouth theme" \
    "cp -a $BOOTSTRAP_DIR/themes/screeninvader $CHROOT_DIR/usr/share/plymouth/themes/"

  check "Copy xosd lib" \
    "cp -a $BOOTSTRAP_DIR/build/xosd-2.2.14/src/libxosd/.libs/libxosd.so.2.2.14 $CHROOT_DIR/usr/lib/"

  check "ldconfig" \
    "$CHRT ldconfig"

  check "Copy osd binary"  \
    "cp -a $BOOTSTRAP_DIR/build/xosd-2.2.14/src/xmms_plugin/osd $CHROOT_DIR/lounge/bin/"

  check "Update plymouth theme" \
    "$CHRT plymouth-set-default-theme -R screeninvader"

}

function doCleanup() {
  check "Clean apt cache" \
    "$CHRT apt-get clean"

  check "Remove black listed files" \
    "$CHRT bash -c \"rm -fr $FILES_BLACK\""
}

function doPrepareChroot() {
  cd "$CHROOT_DIR"
  mount --bind /dev/ dev
  mount -t proc none proc
  mount -t sysfs none sys
  mount -t tmpfs none tmp
  mount -t devpts none dev/pts

  mkdir -p "$CHROOT_DIR/etc/apt/"

  $BOOTSTRAP_DIR/templates/sources_list "$EMDEBIAN_MIRROR" "$DEBIAN_MIRROR" "$DEBIAN_MULTIMEDIA_MIRROR" > $CHROOT_DIR/etc/apt/sources.list

  if [ -n "$APTCACHER_PORT" ]; then
    # use apt-cacher-ng to cache packages during install
    mkdir -p "$CHROOT_DIR/etc/apt/apt.conf.d/"
    $BOOTSTRAP_DIR/templates/00aptcacher "$APTCACHER_PORT" > $CHROOT_DIR/etc/apt/apt.conf.d/00aptcacher
  fi

  # disable starting daemons after install
  mkdir -p "$CHROOT_DIR/usr/sbin"
  $BOOTSTRAP_DIR/templates/policy-rc_d > "$CHROOT_DIR/usr/sbin/policy-rc.d"
  chmod 755 "$CHROOT_DIR/usr/sbin/policy-rc.d"
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
export CHROOT_DIR="`absPath $1`"
export CHRT="chroot \"$CHROOT_DIR\" "

if [ $# -ne 1 ]; then
  printUsage
else
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
  trap doFreeChroot SIGINT SIGTERM EXIT

  if [ -z "$NOINSTALL" ]; then 
    doPackageConf
  else
    skip "package configuration"
  fi

  doCopy
  doCleanup
fi


