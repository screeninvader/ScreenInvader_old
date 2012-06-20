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



function wgeto {
  wget -O - -q "$1"
}

if [ $# -ne 2 ]; then
   echo "Usage: arte7get.sh <videopageurl> <outputfile>" 1>&2
   exit 1
fi

XML_URL=`wgeto "$1" | fgrep "vars_player.videorefFileUrl" | cut -d"\"" -f2`
XML_DE_URL=`wgeto "$XML_URL" | fgrep "<video lang=\"de\"" | cut -d"\"" -f4`
RTMP_URL=`wgeto "$XML_DE_URL" | sed -n -e 's/.*<url quality="hd">\(.*\)<\/url>.*/\1/p'`
flvstreamer -r "$RTMP_URL" > "$2"


