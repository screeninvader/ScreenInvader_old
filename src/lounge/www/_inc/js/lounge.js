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
  showWidget.loadInto('#show_panel');
  findWidget.loadInto('#find_panel');
  playerWidget.loadInto('#player_panel');
  navigateWidget.loadInto('#navigate_panel');  
  uploadWidget.loadInto('#upload_panel');
  playlistWidget.loadInto('#playlist_panel');
  closeWidget.loadInto('#close_panel');

  widgets = [
   soundWidget,
   showWidget,
   findWidget,
   playerWidget,
   navigateWidget,
   uploadWidget,
   playlistWidget,
   closeWidget
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


