
function checkRunning() {
  $.get("cgi-bin/mpd/isRunning", function(data){
    if(data == "true") {
	$("#currentsong").show();
	if ($('#chattable').attr('_hidden') == "true") {
	  $("#nextsongs").show();
        }
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

    $('#chat').css('height', (screen.height - 460) + 'px' );
  });
  $('#chat').scrollTop = $('#chat').scrollHeight;
}

function showChat() {
  $("#nextsongs").hide();
  $("#chattable").show();
  $('#chattable').attr('_hidden', 'false');
  $('#chat').scrollTop = $('#chat').scrollHeight;
//  $.get("cgi-bin/unblank", function(data){
//  });

}

function hideChat() {
  $.get("cgi-bin/mpd/isRunning", function(data){
    $("#chattable").hide();
    if(data == "true") {
      $("#nextsongs").show();
    }
   $('#chattable').attr('_hidden', 'true');
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
  hideChat();
  checkRunning();
  setInterval("checkRunning()", 3000);
});


