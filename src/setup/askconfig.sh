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

ask="dialog --stdout --ok-label Next --cancel-label Back"

function askHowNet() {
  $ask --radiolist "Network Configuration" 11 42 4 \
        dhcp "Automatic configuration" on \
        manual "Manual configuration" off
}

function askNetConnection() {
  $ask --radiolist "Network Connection" 9 32 2 \
    Ethernet "Wired" on \
    Wifi "Wireless" off
}

function askWifiSSID() {
  $ask --nocancel --inputbox "Wireless Network (SSID)" 7 30
}

function askWifiEncryption() {
  $ask  --radiolist "Wireless Encryption" 10 38 3 \
    WPA-PSK "WPA Passphrase" on \
    WEP "WEP Passphrase" off \
    NONE "No encryption"  off
}

function askWifiPassphrase() {
  $ask --insecure --passwordbox "Wireless Passphrase" 7 30
}


function askHostname() {
  $ask --nocancel --inputbox "Please specify a hostname:" 7 30
}

function askManualNetwork() {
  $ask  --form "Manual Network Configuration" 12 38 0 \
        "Address:"      1 1 "" 1 12 20 0 \
        "Netmask:"      2 1 "" 2 12 20 0 \
        "Gateway:"      3 1 "" 3 12 20 0 \
        "DNS:"          4 1 "" 4 12 20 0
}

function askDoReboot() {
  $ask --yesno 'Finish configuration and reboot?' 6 43
}

