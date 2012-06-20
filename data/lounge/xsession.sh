export DISPLAY=:0
setterm -blank 0 -powersave off -powerdown 0 &
xhost + &
xset dpms force on
nice -n19 midori -c /lounge/.config/midori_mpdstat/ --class="MPDstat" --name="MPDstat" http://localhost/mpdstat.html &
sudo -u lounge awesome

