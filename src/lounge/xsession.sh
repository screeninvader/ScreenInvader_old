export DISPLAY=:0

killall janosh -d &
sudo /lounge/bin/gainroot /lounge/bin/janosh -d &
sudo -u lounge /lounge/bin/janosh -d &

/root/triggers/display apply

sudo -u lounge xhost +
sudo -u lounge /lounge/bin/showip &>/dev/null &
sudo -u lounge /lounge/bin/player mplayer_start &>/dev/null &
sudo -u lounge /lounge/bin/player refresh

# give midori a fixed geometry because sometimes it fails to adapt to resolution changes
exec evilwm -bw 0 -fn "-misc-topaz a500a1000a2000-medium-r-normal--0-240-0-0-c-0-iso8859-1"  -app midori -g "$(/lounge/bin/janosh -r get /display/resolution/value)"+0+0

