
function checkRunning() {
  $.get("cgi-bin/mpd/isRunning", function(data){
    if(data == "true") {
	$("#currentsong").show();
	$("#nextsongs").show();
        $("#stopped").hide();
    } else if(data == "false") {
      $("#currentsong").hide();
      $("#nextsongs").hide();
      $("#stopped").show();
    }
  });
}

function updateTitle() {
  $.get("cgi-bin/mpd/title", function(data){
    $("#title").text(data);
  });
}

function updateArtist() {
  $.get("cgi-bin/mpd/artist", function(data){
    $("#artist").text(data);
  });
}

function updateAlbum() {
  $.get("cgi-bin/mpd/album", function(data){
    $("#album").text(data);
  });
}

function updateTime() {
  $.get("cgi-bin/mpd/time", function(data){
    $("#time").text(data);
  });
}

function updateTrack() {
  $.get("cgi-bin/mpd/track", function(data){
    $("#track").text(data);
  });
}

function updatePlaylist() {
  $.get("cgi-bin/mpd/playlist", function(data){
    $("#playlist").html(data);
  });
}

$(document).ready( function ($) {
  $("#stopped").hide();
  updateTitle();
  updateTrack();
  updateTime();
  updateArtist();
  updateAlbum();
  updatePlaylist();

  setInterval("checkRunning()", 3000);
  setInterval("updateTitle()", 3000);	
  setInterval("updateTrack()", 3000);
  setInterval("updateTime()", 3000);
  setInterval("updateArtist()", 3000);
  setInterval("updateAlbum()", 3000);
  setInterval("updatePlaylist()", 6000);

});


