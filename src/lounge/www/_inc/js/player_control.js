function PlayerControl () {
  this.init = function() {
    $('#player-control #previous').live ('click', function(){
      $.get('cgi-bin/mplayer/prev');
    })

    $('#player-control #rewindmore').live ('click', function(){
      $.get('cgi-bin/mplayer/rewindmore');
    })

    $('#player-control #rewind').live ('click', function(){
      $.get('cgi-bin/mplayer/rewind');
    })

    $('#player-control #pause').live ('click', function(){
      $.get('cgi-bin/mplayer/pause');
    })

    $('#player-control #forward').live ('click', function(){
      $.get('cgi-bin/mplayer/forward');
    })

    $('#player-control #forwardmore').live ('click', function(){
      $.get('cgi-bin/mplayer/forwardmore');
    })

    $('#player-control #subtitle').live ('click', function(){
      $.get('cgi-bin/mplayer/togglesub');
    })

    $('#player-control #osd').live ('click', function(){
      $.get('cgi-bin/mplayer/osd');
    })

    $('#player-control #next').live ('click', function(){
      $.get('cgi-bin/mplayer/next');
    })
  };
  
  this.update = function(ScreenInvader) {
  };
  
  this.loadInto = function(into) {
    return $.get(lounge.prefix + 'player_control.html', function(data) {
      $(into).prepend(data);
    }, "text");
  };
}

playerWidget = new PlayerControl();
playerWidget.init();

