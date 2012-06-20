#!/bin/bash
dir="`dirname $0`"
MAKEPARTITION_DIR="`cd $dir; pwd`"
DEVICE="$1"
SIZE="$2"

function printUsage() {
	cat 1>&2 <<EOUSAGE
makestick - Prepare a file system for installation of the Lounge Media Center

Usage: $0 <device-file> [<sizeInM>]

<device-file>    a block special device.
<sizeInM>        the size of the resulting file system in megabytes.
EOUSAGE 

  exit 1
}

function makeSyslinuxConf() {
  cat > "$1/boot/syslinux/syslinux.cfg" <<EOSYSLINUX
DEFAULT loungemc
LABEL loungemc
  SAY Booting Lounge Media Center
	KERNEL /vmlinuz
	APPEND quiet splash ro root=UUID=`blkid $DEVICE*1	| cut -d "\"" -f2` initrd=/initrd.img
EOSYSLINUX
}

[ $# -eq 0 -o $# -gt 2 ] && printUsage;
[ ! -b "$DEVICE" ] &&  error "Not a block device: $DEVICE";
[ -z "$SIZE" ] && SIZE=400
[ printf "%d" $SIZE &> /dev/null -o $SIZE -lt 100 ] && error "Invalid size: $SIZE"

export BOOTSTRAP_LOG="makepartition.log"

source "$MAKEPARTITION_DIR/.functions.sh"

check "Write zeros to device" \
  "dd if=/dev/zero of=$DEVICE bs=1M count=$[$SIZE + 1]" 

check "Make disk label" \
  "parted -s $DEVICE mklabel msdos"

check "Make partition" \
  "parted -s $DEVICE mkpart primary ext4 0 ${SIZE}M"

check "Make filesystem" \
  "mkfs.ext4 $DEVICE*1"

tmpdir=`mktemp  -d`
check "Make temporary mount dir" \
  "[ $? -q 0 ]"

check "Mount file system" \
	"mount $DEVICE*1 $tmpdir"

check "Prune syslinux dir" \
  "mkdir -p $tmpdir/boot/syslinux/"

check "Instal extlinux" \
  "extlinux --install  $tmpdir/boot/syslinux"

makeSyslinuxConf "$tmpdir"
check "Make syslinux.cfg" \
  "[ $? -eq 0 ]"

check "Umount file system" \
	"umount $DEVICE*1"

rmdir $tmpdir
check "Remove temporary mount dir" \
  "rmdir $tmpdir"

check "Install syslinux mbr" \
				  "printf '\1' | cat /usr/share/syslinux/altmbr.bin - | dd bs=440 count=1 conv=notrunc of=$DEVICE"

check "Check file system" \
  "fsck.ext4 -fa $DEVICE*1"

