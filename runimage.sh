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
RUNIMAGE_DIR="`cd $dir; pwd`"

export BOOTSTRAP_LOG="runimage.log"
source "$RUNIMAGE_DIR/.functions.sh"

function doRunQemu() {
  check "Starting $1" \
    "true"
  $1 -soundhw ac97 -sdl -enable-kvm -hda $IMAGE_FILE -net user,hostfwd=tcp::5555-:80,hostfwd=tcp::5556-:22 -net nic -m 256
  exit $?
}

function printUsage() {
  cat 1>&2 <<EOUSAGE
runimage.sh - Run a disk image of the ScreenInvader system in qemu

Usage: $0 <image>
<image>           the disk image file
EOUSAGE
  exit 1
}

[ $# -ne 1 ] && printUsage

IMAGE_FILE="$1"

check "Exists image file $IMAGE_FILE" \
  "test -f $IMAGE_FILE"

which qemu > /dev/null && doRunQemu qemu
which qemu-system-`uname -i` > /dev/null && doRunQemu qemu-system-`uname -i`

check "Starting qemu" \ 
  "false"
