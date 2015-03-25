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

#KEYRINGS="`getConf config/keyrings`"
PKG_WHITE="`getConf config/packages_white`"
PKG_EXTRA="`getConf config/packages_extra`"
PKG_BLACK="`getConf config/packages_black`" 
PKG_BUILD="`getConf config/packages_build`"
PKG_SID="`getConf config/packages_sid`"
FILES_BLACK="`getConf config/files_black`"

export LC_ALL="C"

APTNI="apt-get -q -y --no-install-recommends --force-yes -o Dpkg::Options::=\"--force-confdef\" -o Dpkg::Options::=\"--force-confold\" ";

DEBIAN_MIRROR="http://ftp.at.debian.org/debian/"

dir="`dirname $0`"
BOOTSTRAP_DIR="`cd $dir; pwd`"
BOOTSTRAP_LOG="$BOOTSTRAP_DIR/bootstrap.log"
ARCH=armhf
APTCACHER_PORT=
NOINSTALL=
NODEBOOT=
NOCLEANUP=
CHROOT_DIR=
CHRT=
DEBUG=

function printUsage() {
  cat 0>&2 <<EOUSAGE
Bootstrap a ScreenInvader file system.

$0 [-a <arch>][-g <num>][-l <logfile>][-p <apt-cacher-port>][-c <configfile>][-i -d -u -x] <bootstrapdir>
Options:
  -a <arch> Bootstrap a system of the given architecture
  -l <file> Specify the log file
  -p <port> Enables using apt-cacher-ng on the specified port
  -i        Don't configure and install packages
  -d        Don't debootstrap
  -u        Combined -d and -i
  -c <file> Specify the config file for non-interactive configuration at first boot
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

function skip() {
  echo -n "$1: "
  yellow "skipped\n"
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
    "debootstrap  --foreign --variant=minbase --exclude="`echo $PKG_BLACK | sed 's/ /,/g'`" --arch $ARCH wheezy "$CHROOT_DIR" $BOOTSTRAP_MIRROR"

  check "Copy qemu-static" \
    "[ ! -f \"$CHROOT_DIR/usr/bin/qemu-arm-static\" ] && cp /usr/bin/qemu-arm-static \"$CHROOT_DIR/usr/bin\""

  check "Boostrap second stage" \
    "DEBIAN_FRONTEND=noninteractive DEBCONF_NONINTERACTIVE_SEEN=true LC_ALL=C LANGUAGE=C LANG=C chroot \"$CHROOT_DIR\" /debootstrap/debootstrap --second-stage"

  check "Trigger post install" \
    "DEBIAN_FRONTEND=noninteractive DEBCONF_NONINTERACTIVE_SEEN=true LC_ALL=C LANGUAGE=C LANG=C chroot \"$CHROOT_DIR\" dpkg --configure -a"
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

  check "Copy ScreenInvader repo key" \
    "cp $BOOTSTRAP_DIR/repo/ScreenInvaderRepoKey.gpg $CHROOT_DIR/tmp/"

  check "Add ScreenInvader repo key" \
    "$CHRT apt-key add /tmp/ScreenInvaderRepoKey.gpg"

#  check "Install keyrings" \
#    "$CHRT $APTNI install $KEYRINGS"

  check "Update Repositories" \
    "$CHRT $APTNI update"

  check "Update apt policy" \
    "$CHRT bash -c 'touch /var/lib/apt/lists/*; apt-cache policy'"

  check "Install white packages" \
    "$CHRT $APTNI -t wheezy install $PKG_WHITE"

 check "Install sid packages" \
  "$CHRT $APTNI -t sid install $PKG_SID"

#  check "Upgrade packages" \
#    "$CHRT $APTNI upgrade"

  check "Remove black listed packages" \
    "$CHRT $APTNI purge $PKG_BLACK"
}

function doBuild() {
  check "Install build dependencies" \
    "$CHRT $APTNI -t sid install $PKG_BUILD"

  check "Clone dri2" \
    "cd $BOOTSTRAP_DIR/third/; ./clone_dri2.sh"

#  check "Clone janosh" \
#    "cd $BOOTSTRAP_DIR/third/; ./clone_janosh.sh"

#  check "Clone luajitrocks" \
#    "cd $BOOTSTRAP_DIR/third/; ./clone_luajitrocks.sh"

  check "Clone libvdpau" \
    "cd $BOOTSTRAP_DIR/third/; ./clone_libvdpau-sunxi.sh"

  check "Clone SimpleOSD" \
    "cd $BOOTSTRAP_DIR/third/; ./clone_simpleosd.sh"

  check "Clone sunxi-mali" \
    "cd $BOOTSTRAP_DIR/third/; ./clone_sunxi-mali.sh"

#  check "Clone sunxi-tools" \
#   "cd $BOOTSTRAP_DIR/third/; ./clone_sunxi-tools.sh"

  check "Clone libump" \
    "cd $BOOTSTRAP_DIR/third/; ./clone_ump.sh"

  check "Clone fbturbo" \
    "cd $BOOTSTRAP_DIR/third/; ./clone_xf86-video-fbturbo.sh"

  check "Copy third party" \
    "cp -r $BOOTSTRAP_DIR/third/ \"$CHROOT_DIR\""

  check "build dri2" \
    "$CHRT /third/build_dri2.sh"

  check "build sunxi-mali" \
    "$CHRT /third//build_sunxi-mali.sh"

#  check "build sunxi-tools" \
#    "$CHRT /third/build_sunxi-tools.sh"

  check "build xf86-video-fbturbo" \
    "$CHRT /third/build_xf86-video-fbturbo.sh"

  check "build libvdpau-sunxi" \
    "$CHRT /third/build_libvdpau-sunxi.sh"

#  check "build janosh" \
#    "$CHRT /third/build_janosh.sh"

  check "build SimpleOSD" \
    "$CHRT /third/build_simpleosd.sh"

  check "remove build dependencies" \
    "$CHRT $APTNI remove $PKG_BUILD"
}

function doCopy() {
  check "Make install directory" \
    "mkdir -p $CHROOT_DIR/install/"

  check "Copy debian packages" \
    "cp $BOOTSTRAP_DIR/packaging/*.deb $CHROOT_DIR/install/"
  
#  check "Install mplayer" \
#    "cd $BOOTSTRAP_DIR/third/MPlayer-1.1.1/; make DESTDIR=$CHROOT_DIR/ install"

  #check "Install sunxi-mali" \
    #"cd $BOOTSTRAP_DIR/third/sunxi-mali/; make DESTDIR=$CHROOT_DIR/ install"

  #check "Install sunxi-tools" \
  #  "cd $BOOTSTRAP_DIR/third/sunxi-tools/; make DESTDIR=$CHROOT_DIR/ install"

#  check "Install libdri2" \
#    "cd $BOOTSTRAP_DIR/third/libdri2/; make DESTDIR=$CHROOT_DIR/ install"

#  check "Install xf86-video-fbturbo" \
#    "cd $BOOTSTRAP_DIR/third/xf86-video-fbturbo/; make DESTDIR=$CHROOT_DIR/ install"

#  check "Install libump" \
#    "cd $BOOTSTRAP_DIR/third/libump/; make DESTDIR=$CHROOT_DIR/ install"
  
#  check "Install libvdpau-sunxi" \
#    "cd $BOOTSTRAP_DIR/third/libvdpau-sunxi/; make DESTDIR=$CHROOT_DIR/ install"
 
#  check "Install janosh" \
#    "cd $BOOTSTRAP_DIR/third/Janosh/; make PREFIX=/lounge/bin DESTDIR=$CHROOT_DIR/ install"
 
 # check "install kernel package" \
#    "$CHRT dpkg -i /install/screeninvader-kernel-all.deb"

  check "install core package" \
    "$CHRT dpkg -i /install/screeninvader-core-all.deb"

  check "install config package" \
    "$CHRT dpkg -i --force-all /install/screeninvader-config-all.deb"

  check "install misc package" \
    "$CHRT dpkg -i --force-all /install/screeninvader-misc-all.deb"

  check "install binaries package" \
    "$CHRT dpkg -i /install/screeninvader-binaries-all.deb"

  check "Remove install directory" \
    "rm -r $CHROOT_DIR/install/"

#  check "Copy plymouth theme" \
#    "cp -a $BOOTSTRAP_DIR/themes/screeninvader $CHROOT_DIR/usr/share/plymouth/themes/"

  check "ldconfig" \
    "$CHRT ldconfig"

#  check "Update plymouth theme" \
#    "$CHRT plymouth-set-default-theme -R screeninvader"

  if [ -n "$CONFIG_FILE" ]; then
    check "Copy firstboot config file" \
      "cp $CONFIG_FILE \"$CHROOT_DIR/setup/answer.sh\""
  fi
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
    "\"$BOOTSTRAP_DIR/templates/sources_list\" \"$DEBIAN_MIRROR\" > \"$CHROOT_DIR/etc/apt/sources.list\""

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

  if [ -n "${CONFIG_FILE}" ]; then
    check "Check firstboot config file" \
      "[ -f \"${CONFIG_FILE}\" ]"
  fi
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

while getopts 'a:l:p:g:c:iduxbz' c
do
  case $c in
    a) ARCH="$OPTARG";;
    c) CONFIG_FILE="`absPath $OPTARG`";;
    l) BOOTSTRAP_LOG="`absPath $OPTARG`";;
    p) APTCACHER_PORT="$OPTARG";;
    i) NOINSTALL="YES";;
    d) NODEBOOT="YES";;
    z) NOCLEANUP="YES";;
    u) NOINSTALL="YES"; NODEBOOT="YES"; NOCLEANUP="YES";;
    x) INSTALL_EXTRA="YES";;
    b) DONT_REBUILD="YES";;
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

  if [ -z "$NOCLEANUP" ]; then
    doCleanupPackages
  else
    skip "cleanup packages"
  fi

  if [ -z "$DONT_REBUILD" ]; then 
    doBuild
  else
    skip "build"
  fi

  doCleanupFiles
  doCopy
  doCreateBuildHtml
fi


exit 0

