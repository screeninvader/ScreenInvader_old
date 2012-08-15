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
LOOPBACK_DEVICE=`losetup -f`
CHROOT_DIR="../chrootdir"

check "Creating disk image file $IMAGE_FILE" \
  "dd if=/dev/zero of=$IMAGE_FILE bs=1024 count=2072000 > /dev/null"

check "Setting up disk image file on loopback device $LOOPBACK_DEVICE" \
  "losetup $LOOPBACK_DEVICE $IMAGE_FILE"

./makestick.sh "$LOOPBACK_DEVICE" "$@"

OFFSET=`parted -s -m $LOOPBACK_DEVICE unit B print | grep "^1:" | cut -f 2 -d ":" | tr B ' '`

sync
losetup -d "$LOOPBACK_DEVICE"

mount $IMAGE_FILE $CHROOT_DIR -o loop,offset=$OFFSET
