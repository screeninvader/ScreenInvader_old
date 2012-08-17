#!/bin/sh

### BEGIN INIT INFO
# Provides:          mpd
# Required-Start:    $local_fs $remote_fs
# Required-Stop:     $local_fs $remote_fs
# Should-Start:      autofs $network alsa-utils pulseaudio
# Should-Stop:       autofs $network alsa-utils pulseaudio
# Default-Start:     2 3 4 5
# Default-Stop:      0 1 6
# Short-Description: Music Player Daemon
# Description:       Start the Music Player Daemon (MPD) service
#                    for network access to the local audio queue.
### END INIT INFO

. /lib/lsb/init-functions

PATH=/sbin:/bin:/usr/sbin:/usr/bin
NAME=mpd
DESC="Music Player Daemon"
DAEMON=/usr/bin/mpd
MPDCONF=/etc/mpd.conf
START_MPD=true

# Exit if the package is not installed
[ -x "$DAEMON" ] || exit 0

# Read configuration variable file if it is present
[ -r /etc/default/$NAME ] && . /etc/default/$NAME

if [ -n "$MPD_DEBUG" ]; then
    set -x
    MPD_OPTS=--verbose
fi

mkdir -p /var/run/mpd
chmod a+rw /var/run/mpd

DBFILE=$(sed -n 's/^[[:space:]]*db_file[[:space:]]*"\?\([^"]*\)\"\?/\1/p' $MPDCONF)
PIDFILE=$(sed -n 's/^[[:space:]]*pid_file[[:space:]]*"\?\([^"]*\)\"\?/\1/p' $MPDCONF)

mpd_start () {
    if [ "$START_MPD" != "true" ]; then
        log_action_msg "Not starting MPD: disabled by /etc/default/$NAME".
        exit 0
    fi

    log_daemon_msg "Starting $DESC" "$NAME"

    if [ -z "$PIDFILE" -o -z "$DBFILE" ]; then
        log_failure_msg \
            "$MPDCONF must have db_file and pid_file set; cannot start daemon."
        exit 1
    fi

    PIDDIR=$(dirname "$PIDFILE")
    if [ ! -d "$PIDDIR" ]; then
        mkdir -m 0755 $PIDDIR
        chown mpd:audio $PIDDIR
    fi

    if [ "$FORCE_CREATE_DB" -o ! -f "$DBFILE" ]; then
        log_warning_msg "creating $DBFILE... "
        $DAEMON --create-db "$MPDCONF" > /dev/null 2>&1
    fi

    start-stop-daemon --start --quiet --oknodo --pidfile "$PIDFILE" \
        --exec "$DAEMON" -- $MPD_OPTS "$MPDCONF"
    log_end_msg $?
}

mpd_stop () {
    if [ "$START_MPD" != "true" ]; then
        log_failure_msg "Not stopping MPD: disabled by /etc/default/$NAME".
        exit 0
    fi
    if [ -z "$PIDFILE" ]; then
        log_failure_msg \
            "$MPDCONF must have pid_file set; cannot stop daemon."
        exit 1
    fi

    log_daemon_msg "Stopping $DESC" "$NAME"
    start-stop-daemon --stop --quiet --oknodo --retry 5 --pidfile "$PIDFILE" \
        --exec $DAEMON
    log_end_msg $?
}

# note to self: don't call the non-standard args for this in
# {post,pre}{inst,rm} scripts since users are not forced to upgrade
# /etc/init.d/mpd when mpd is updated
case "$1" in
    start)
        mpd_start
        ;;
    stop)
        mpd_stop
        ;;
    restart|force-reload)
        mpd_stop
        mpd_start
        ;;
    force-start|start-create-db)
        FORCE_CREATE_DB=1
        mpd_start
        ;;
    force-restart)
        FORCE_CREATE_DB=1
        mpd_stop
        mpd_start
        ;;
    *)
        echo "Usage: $0 {start|start-create-db|stop|restart}"
        exit 2
        ;;
esac
