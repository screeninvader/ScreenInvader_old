jQuery.fn.wait = function (MiliSeconds) {
  $(this).animate({ opacity: '+=0' }, MiliSeconds);
  return this;
}

function Lounge(local) {
 this.local = local;
  
  this.init = function() {
    this.prefix = location.href.replace(/\/[^\/]*$/, "") + "/";
  }
}

var lounge = new Lounge(true);
var video;
var ScreenInvader = {};

function bookmarklet() {
    $.get('cgi-bin/bookmarklet', function(data) {
      $("#bookmarklet").attr("href",data);
    }, 'text');
}


function updateScreenInvader() {
  $.getJSON("cgi-bin/get?/.", function(data) {
    $.each(widgets, function(i,w) {
      w.update(data);
    });

  });
}

$().ready(function () {
  lounge.init();
  soundWidget.loadInto('#sound_panel');
  screenWidget.loadInto('#screen_panel');
  showWidget.loadInto('#show_panel');
  findWidget.loadInto('#find_panel');
  cleanupWidget.loadInto('#cleanup_panel');
  playerWidget.loadInto('#player_panel');
  navigateWidget.loadInto('#navigate_panel');  
  uploadWidget.loadInto('#upload_panel');
  playlistWidget.loadInto('#playlist_panel');

  widgets = [
   soundWidget,
   screenWidget,
   showWidget,
   findWidget,
   cleanupWidget,
   playerWidget,
   navigateWidget,
   uploadWidget,
   playlistWidget
  ]

  $('.button').ready(function() {
    $(this).addClass('up');
  });


  $('.button').live('mousedown', function(){
   $(this).addClass('down');
   $(this).removeClass('up');
  }).live('mouseup', function(){
   $(this).addClass('up');
   $(this).removeClass('down');
  })

  bookmarklet();
  updateScreenInvader();
  setInterval("updateScreenInvader()",1000);
});


