#!/bin/bash

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


