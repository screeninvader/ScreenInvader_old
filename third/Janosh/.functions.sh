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

#!/bin/bash
function black { echo -ne "\033[0;30m$1\033[0m"; tput sgr0; }
function blue { echo -ne "\033[0;34m$1\033[0m"; tput sgr0; }
function green { echo -ne "\033[0;32m$1\033[0m"; tput sgr0; }
function cyan { echo -ne "\033[0;36m$1\033[0m"; tput sgr0; }
function red { echo -ne "\033[1;31m$1\033[0m"; tput sgr0; }
function purple { echo -ne "\033[0;35m$1\033[0m"; tput sgr0; }
function brown { echo -ne "\033[0;33m$1\033[0m"; tput sgr0; }
function lightgray { echo -ne "\033[0;37m$1\033[0m"; tput sgr0; }
function lightgreen { echo -ne "\033[1;32m$1\033[0m"; tput sgr0; }
function lightcyan { echo -ne "\033[1;36m$1\033[0m"; tput sgr0; }
function lightred { echo -ne "\033[0;31m$1\033[0m"; tput sgr0; }
function lightpurple { echo -ne "\033[1;35m$1\033[0m"; tput sgr0; }
function yellow { echo -ne "\033[1;33m$1\033[0m"; tput sgr0; }
function white { echo -ne "\033[1;37m$1\033[0m"; tput sgr0; }
function warn { red "$1\n"; }
function ok { green "ok\n"; }
function failed { red "failed\n"; }
function verbose { [ ! -z $VERBOSE ] && echo "$@ " 1>&2; }
function verboseexec { verbose $@; bash -c "$@"; }
function error { 
    msg=$1; 
    errcode=$2; 
    [ -z "$errcode" ] && errcode=1; 
    [ -z "$msg" ] && failed || red "$msg\n"; 
    exit $errcode; 
}

function try {
  fatal=true
  targetfd=2
  OPTIND=0
  while getopts 'no:' c
  do
    case $c in
      n)fatal="false";;
      o)targetfd="$OPTARG";;
      \?)echo "Invalid option: -$OPTARG" >&2;;
    esac
  done
  shift $((OPTIND-1));
  errcode=0; 
  msg="$1"
  echo -en "$msg: " 1>&2; 
  shift;

  verbose $@;

  bash -c "$@ 2>&1 1>&$targetfd"
  errcode=$?; 
  if [ $errcode != 0 ]; then 
    [ "$fatal" != "true" ] && failed 1>&2 || error 1>&2;
  else 
    ok 1>&2; 
  fi
  return $errcode; 
}

function check {
  log="&1"
  if [ -n "$BOOTSTRAP_LOG" ]; then
    echo "### $1" >> "$BOOTSTRAP_LOG"
    try -o "1" "$1" "$2" >>"$BOOTSTRAP_LOG"
  else
    try "$1" "$2"
  fi

  [ $? -ne 0 ] && exit
}

function checkcat {
	try -n -o "1" "$1" "echo; $2"
}


function trycat { 
  try -o "1" "$1" "$2"
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

