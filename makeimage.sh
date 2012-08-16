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

dir="`dirname $0`"
MAKEIMAGE_DIR="`cd $dir; pwd`"

source "$MAKEIMAGE_DIR/.functions.sh"

IMAGE_FILE="tsarbomba.dd"
IMAGE_SIZE=$1
LOOPBACK_DEVICE=`losetup -f`
CHROOT_DIR="../chrootdir"

[ -z $IMAGE_SIZE ] && IMAGE_SIZE="1000"

check "Mountpoint $CHROOT_DIR is unused" \
  "! mountpoint -q $CHROOT_DIR"

check "Creating disk image file $IMAGE_FILE of size $IMAGE_SIZE MB" \
  "dd if=/dev/zero of=$IMAGE_FILE bs=1M count=1 seek=$IMAGE_SIZE > /dev/null"

check "Setting up disk image file on loopback device $LOOPBACK_DEVICE" \
  "losetup $LOOPBACK_DEVICE $IMAGE_FILE"

./makestick.sh "$LOOPBACK_DEVICE" "$@"

OFFSET=`parted -s -m $LOOPBACK_DEVICE unit B print | grep "^1:" | cut -f 2 -d ":" | tr B ' '`

sync

check "Detaching disk image file" \
  "losetup -d $LOOPBACK_DEVICE"

check "Mounting screen invader partition on $CHROOT_DIR" \
  "mount $IMAGE_FILE $CHROOT_DIR -o loop,offset=$OFFSET"
