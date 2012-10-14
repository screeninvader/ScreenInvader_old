
function PlaylistControl () {
  this.queuemode = false;

  this.setQueuemode = function(on) {
    queuemode=on;
    if(on) {
      $('#playlist-control #queue-toggle').text("Queue Mode");
      $('#playlist-control #queue-toggle').addClass("up");
      $('#playlist-control #queue-toggle').removeClass("down");
    } else {
      $('#playlist-control #queue-toggle').text("Post Mode"); 
      $('#playlist-control #queue-toggle').addClass("down");
      $('#playlist-control #queue-toggle').removeClass("up");
    }
  };

  this.init = function() {
    $('#playlist-control #queue-toggle').live ('click', function(){
      var q=!queuemode;
      $.get('cgi-bin/set?/playlist/queue=' + q);
      playlistWidget.setQueuemode(q);
    })

    $('#playlist-control #save-button').live('click', function(e) {
      e.preventDefault(); 
      window.location.href = 'cgi-bin/playlist_save';
    })

    $('#playlist-control #clear-button').live ('click', function(){
      $.get('cgi-bin/playlist_clear');
    })
  };
  
  this.update = function(ScreenInvader) {
    playlistWidget.setQueuemode(ScreenInvader.playlist.queue == "true" ? true : false);
  };

  this.loadInto = function(into) {
    return $.get(lounge.prefix + 'playlist_control.html', function(data) {
      $(into).prepend(data);
    }, "text");
  };
}

playlistWidget = new PlaylistControl();
playlistWidget.init();

