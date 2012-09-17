function PlayerControl () {
  this.init = function() {
    $('#player-control #previous').live ('click', function(){
      $.get('cgi-bin/trigger?playerPrevious');
    })

    $('#player-control #rewindmore').live ('click', function(){
      $.get('cgi-bin/trigger?playerRewindMore');
    })

    $('#player-control #rewind').live ('click', function(){
      $.get('cgi-bin/trigger?playerRewind');
    })

    $('#player-control #pause').live ('click', function(){
      $.get('cgi-bin/trigger?playerPause');
    })

    $('#player-control #forward').live ('click', function(){
      $.get('cgi-bin/trigger?playerForward');
    })

    $('#player-control #forwardmore').live ('click', function(){
      $.get('cgi-bin/trigger?playerForwardMore');
    })

    $('#player-control #subtitle').live ('click', function(){
      $.get('cgi-bin/trigger?playerToggleSub');
    })

    $('#player-control #osd').live ('click', function(){
      $.get('cgi-bin/trigger?playerToggleOSD');
    })

    $('#player-control #next').live ('click', function(){
      $.get('cgi-bin/trigger?playerNext');
    })
  };
  
  this.update = function(ScreenInvader) {
    if(ScreenInvader.player.active == "true") {
      $('#player-control').css('display', '');
    }else {
      $('#player-control').css('display', 'none');
    }
  };
  
  this.loadInto = function(into) {
    return $.get(lounge.prefix + 'player_control.html', function(data) {
      $(into).prepend(data);
    }, "text");
  };
}

playerWidget = new PlayerControl();
playerWidget.init();

