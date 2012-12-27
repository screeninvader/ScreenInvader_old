export DISPLAY=:0
mkdir -p /var/run/player/
chown lounge:lounge /var/run/player/
sudo -u lounge /lounge/triggers/sound reload
/root/triggers/display apply

sudo -u lounge xhost +
sudo -u lounge /lounge/bin/showip &>/dev/null &
sudo -u lounge /lounge/bin/player mplayer_start &>/dev/null &
sudo -u lounge /lounge/bin/player refresh

# give midori a fixed geometry because sometimes it fails to adapt to resolution changes
killall evilwm
exec evilwm -bw 0 -fn "-misc-topaz a500a1000a2000-medium-r-normal--0-240-0-0-c-0-iso8859-1"  -app midori -g "$(/lounge/bin/janosh -r get /display/resolution/value)"+0+0

