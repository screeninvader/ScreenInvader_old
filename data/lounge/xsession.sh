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

export DISPLAY=:0
sudo -u lounge bash -c "sleep 1; xdotool mousemove 1900 1200" 
sudo -u lounge setterm -blank 0 -powersave off -powerdown 0
sudo -u lounge xset s off
sudo -u lounge xhost +
sudo -u lounge /lounge/bin/showip &>/dev/null &
sudo -u lounge /usr/bin/awsetbg -c /lounge/www/_inc/img/logo.png
sudo -u lounge /lounge/bin/player mplayer_start &>/dev/null &
sudo -u lounge /lounge/bin/player refresh &> /dev/null &
sudo -u lounge awesome

