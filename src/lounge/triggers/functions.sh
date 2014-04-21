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

export DISPLAY=:0
export HOME=/lounge
export USER=lounge
export PATH="$HOME/bin:$PATH"
export LOGDIR="/var/log/lounge"

l_log () {
 echo "`date +"%D %T"`: $0 $@" >> /tmp/screeninvader.log
} 

function l_urlencode() {
 echo "$1" | sed 's/ /%20/g;s/!/%21/g;s/"/%22/g;s/#/%23/g;s/\$/%24/g;s/\&/%26/g;s/'\''/%27/g;s/(/%28/g;s/)/%29/g;s/:/%3A/g'
}

function l_urldecode() {
  echo $1 | sed -e's/%\([0-9A-F][0-9A-F]\)/\\\\\x\1/g' | xargs echo -e | sed 's/+/ /g'
}

function l_unblank() {
  xdotool mousemove 1900 1200
  janosh -t set /display/blank false
}

function l_lock() {
  lockname="$1"
  cat | (
  flock -n 200 | exit 111
  cat
  ) 200> "/var/lock/$lockname" &> /dev/null
}

function l_notify() {
  timeout=$2
  [ -z "$timeout" ] && timeout=2

  title="$1"
  /lounge/bin/notify "$title" 
  #"$timeout"
}
export -f l_log l_unblank l_notify l_urldecode
