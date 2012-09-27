#!/bin/bash

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
    NONE "No encryptiion"  off
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

