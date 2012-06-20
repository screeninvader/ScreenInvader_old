#!/bin/bash
[ -z "$LC_ALL" ] && export LC_ALL=C
cd `dirname $0`

ask="dialog --stdout --ok-label Next --cancel-label Back"

function askHowNetConf() {
  $ask --radiolist "Network Configuration" 11 42 4 \
	dhcp "Automatic configuration" on \
	manual "Manual configuration" off
}

function askHostnameConf() {
  $ask --nocancel --inputbox "Please specify a hostname:" 7 30
}

function askManualNetworkConf() {
  $ask  --form "Manual Network Configuration" 12 38 0 \
	"Address:"	1 1 "" 1 12 20 0 \
	"Netmask:"	2 1 "" 2 12 20 0 \
	"Gateway:"	3 1 "" 3 12 20 0 \
	"DNS:"		4 1 "" 4 12 20 0
}

function askDoSMBConf() {
  $ask --yesno 'Would you like to configure a network share?' 6 48
}

function askSMBConf() {
  $ask --form "Samba (Windows Network) Configuration" 12 38 0 \
	"Workgroup:" 1 1 "WORKGROUP" 1 12 20 0 \
	"Host:" 2 1 "" 2 12 20 0 \
	"Path:" 3 1 "/" 3 12 20 0 \
	"User:" 4 1 "" 4 12 20 0 \
	"Password:" 5 1 "" 5 12 20 0
}

function askDoReboot() {
  $ask --yesno 'Finish configuration and reboot?' 6 43
}

function makeHostnameConf() {
  hostname $1
  echo "$1" > "/etc/hostname"
}

function makeDNSConf() {
  templates/resolv_conf "$1" > /etc/resolv.conf
}

function makeDHCPNetConf() {
  templates/interfaces_dhcp > /etc/network/interfaces
}

function makeManualNetConf() {
  templates/interfaces_man "$1" "$2" "$3" > /etc/network/interfaces
}

function makeSMBConf() {
  templates/auto_smb "$1" "$2" "$3" "$4" "$5" > /etc/auto.smb
  ln -sf "/var/autofs/smb/$4" "/share/$4"
}

function makeInittabConf() {
  templates/inittab > /etc/inittab
}

SCREENS="hostname network smb reboot"

function doConf() {
	${1}Conf
}

function hostnameConf(){
  HOSTNAME=
  while [ -z "$HOSTNAME" ]; do  
    HOSTNAME=$(askHostnameConf)
  done

  makeHostnameConf "$HOSTNAME"
	doConf "network"
}

function networkConf() {
  HOWCONF=$(askHowNetConf)
  if [ $? == 0 ]; then
    if [ "$HOWCONF" == "dhcp" ]; then
      makeDHCPNetConf
    elif  [ "$HOWCONF" == "manual" ]; then
      NETCONF=$(askManualNetworkConf)
      if [ $? == 0 ]; then
        set $NETCONF
        makeManualNetConf "$1" "$2" "$3"
        makeDNSConf "$4"
      else
				doConf "network"
      fi
    fi
  else
    doConf "hostname"
  fi

	doConf "smb"
}

function smbConf() {
  if askDoSMBConf; then
    SMBCONF="$(askSMBConf)"
    if [ $? -eq 0 ]; then
      set $SMBCONF
       makeSMBConf "$4" "$5" "$1" "$2" "$3"
    else
      doConf "network"
    fi
  fi

	doConf "reboot"
}

function rebootConf(){
  if askDoReboot; then
    return 0
  else
   doConf
  fi
}

doConf

update-rc.d autofs defaults
update-rc.d thttpd defaults
update-rc.d mpd defaults
update-rc.d xserver defaults

mkdir -p /share
mkdir -p /var/cache/debconf/
mkdir -p /var/run/mpd/
mkdir -p /var/lib/mpd
chown -R mpd:audio  /var/lib/mpd
chown -R mpd:audio /var/run/mpd/
chmod a+rwx /var/run/mpd/
chown -R lounge:lounge /home/lounge/

usermod -s /bin/bash root
shutdown -r now


