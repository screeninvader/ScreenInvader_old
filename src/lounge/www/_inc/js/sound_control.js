function SoundControl () { 
  this.volume = -320;
  this.bass = 0;
  this.treble = 0;
  this.mute = false;
  this.instance = this;

  this.setVolume = function(level) {
    soundWidget.volume = parseInt(level);
    $('#sound-control #volume #value').text(level);
  }

  this.setBass = function(level) {
    soundWidget.bass = parseInt(level);
    $('#sound-control #bass #value').text(level);
  }

  this.setTreble = function(level) {
    soundWidget.treble = parseInt(level);
    $('#sound-control #treble #value').text(level);
  }

  this.setMute = function(on) {
    if (on.trim() == 'true') {
      text = 'Unmute';
      this.mute = true;
      $('#sound-control #volume #mute').removeClass("up").addClass("down");
    } else {
      text = 'Mute';
      this.mute = false;
      $('#sound-control #volume #mute').addClass("up").removeClass("down");
    }
    $('#sound-control #volume #mute').text(text);
  }

  this.update = function() {
    $.get('cgi-bin/sound/getVolume', function(data) {
      soundWidget.setVolume(data);
    }, 'text');

    $.get('cgi-bin/sound/getBass', function(data) {
      soundWidget.setBass(data);
    }, 'text');

    $.get('cgi-bin/sound/getTreble', function(data) {
      soundWidget.setTreble(data);      
    }, 'text');
    $.get('cgi-bin/sound/isMute', function(data) {
      soundWidget.setMute(data);
    }, 'text');
  };

  this.init = function() {
    $('#sound-control #volume #mute').live( 'click', function() {
      $.get('cgi-bin/sound/toggleMute');
      if(soundWidget.mute)
        soundWidget.setMute('false');
      else
	soundWidget.setMute('true');
    });

    $('#sound-control #volume #minus').live ('click', function(){
      $.get('cgi-bin/sound/volume-less');
      soundWidget.setVolume(soundWidget.volume - 5);
    });

    $('#sound-control #volume #plus').live ('click', function(){
      if(!this.mute) {
      	$.get('cgi-bin/sound/volume-more');
        soundWidget.setVolume(soundWidget.volume + 5);
      }
    });

    $('#sound-control #bass #minus').live ('click', function(){
      $.get('cgi-bin/sound/bass-less');
      soundWidget.setBass(soundWidget.bass - 5);
    });

    $('#sound-control #bass #plus').live ('click', function(){
      $.get('cgi-bin/sound/bass-more');
      soundWidget.setBass(soundWidget.bass + 5);
    });

    $('#sound-control #treble #minus').live ('click', function(){
      $.get('cgi-bin/sound/treble-less');
      soundWidget.setTreble(soundWidget.treble - 5);
    });

    $('#sound-control #treble #plus').live ('click', function(){
      $.get('cgi-bin/sound/treble-more');
      soundWidget.setTreble(soundWidget.treble + 5);
    });
  };
  
  this.loadInto = function(into) {
    return $.get(lounge.prefix + 'sound_control.html', function(data) {
      $(into).prepend(data);
    }, "text");
  };
}


soundWidget = new SoundControl();
soundWidget.init();
soundWidget.update();
setInterval('soundWidget.update()', 3000);
