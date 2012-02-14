
function checkRunning() {
  $.get("cgi-bin/mpd/isRunning", function(data){
    if(data == "true") {
	$("#currentsong").show();
	$("#nextsongs").show();
        $("#stopped").hide();
	updateTitle();
	updateArtist();
	updateAlbum();
	updateTime();
	updateTrack();
	updatePlaylist();
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

function updateLyrics() {
  $.get("cgi-bin/mpd/lyrics", function(data){
    $("#viewer").html(data);
  });
}

$(document).ready( function ($) {
  updateLyrics();
  setInterval("updateLyrics()", 3000);
});


