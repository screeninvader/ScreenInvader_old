#!/bin/bash
(
set -x
export DISPLAY=:0
export HOME=/root
export PATH="/lounge/bin/:$PATH"
mkdir -p /var/run/player/
chown lounge:lounge /var/run/player/
#sudo -u lounge /lounge/triggers/sound reload
/root/triggers/display apply

sudo -u lounge xhost +
#/lounge/bin/networkmon &>>/var/log/networkmon.log &
sudo -u lounge /lounge/bin/showip &>/dev/null &
sudo -u lounge /usr/bin/python /lounge/bin/proxy.py &>/tmp/proxy &

sudo -u lounge /lounge/bin/player mplayer_start &>/dev/null &
sudo -u lounge /lounge/bin/player refresh

# give midori a fixed geometry because sometimes it fails to adapt to resolution changes
killall evilwm

resolution="$(sudo -u lounge /lounge/bin/janosh -r get /display/resolution)"
sudo -u lounge /lounge/bin/janosh -t set /display/resolution $resolution

# apply real resolution
resolution="`xrandr -q | fgrep "*" | tr -s " " | cut -d" " -f2`"
sudo -u lounge /lounge/bin/janosh -t set /display/resolution $resolution

sudo -u lounge cheesy -d -t &> /var/log/cheesy.log &
xset -dpms
xset s off

xbindkeys & 
#sudo -u lounge bash -c "gromit & sleep 3; xdotool key Pause"
awesome 
#exec evilwm -bw 0 -fn "-misc-topaz a500a1000a2000-medium-r-normal--0-240-0-0-c-0-iso8859-1"  -app Midori -g "$resolution"+0+0
) &> /tmp/xsession

