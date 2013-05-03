function SoundControl () { 
  this.volume = -320;
  this.bass = 0;
  this.treble = 0;
  this.mute = false;
  this.instance = this;
  this.blank = false;

  this.setDisplay = function(on) {
    blank=on;
    if(on) {
      $('#sound-control #display').text("Off");
      $('#sound-control #display').addClass("up");
      $('#sound-control #display').removeClass("down");
    } else {
      $('#sound-control #display').text("On");
      $('#sound-control #display').addClass("down");
      $('#sound-control #display').removeClass("up");
    }
  };

  this.setVolume = function(level) {
    volume = parseInt(level);
    $('#sound-control #volume #value').text(level + "%");
  };

  this.setMute = function(on) {
    mute=on;
    if (on) {
      text = 'Unmute';
      $('#sound-control #volume #mute').removeClass("up").addClass("down");
    } else {
      text = 'Mute';
      $('#sound-control #volume #mute').addClass("up").removeClass("down");
    }
    $('#sound-control #volume #mute').text(text);
  };

  this.update = function(ScreenInvader) {
    soundWidget.setVolume(ScreenInvader.sound.volume);
    soundWidget.setMute(ScreenInvader.sound.mute == "true" ? true : false);
    soundWidget.setDisplay(ScreenInvader.display.blank == "true" ? true : false);
  };

  this.init = function() {
    $('#sound-control #display').live ('click', function(){
      var b=!blank;
      $.get('cgi-bin/set?/display/blank=' + b);
      screenWidget.setScreen(b);
    })

    $('#sound-control #volume #mute').live( 'click', function() {
      var m=!mute;
      $.get('cgi-bin/set?/sound/mute=' + m);
      soundWidget.setMute(m);
    });

    $('#sound-control #volume #minus').live ('click', function(){
      var v=volume - 5;
      $.get('cgi-bin/set?/sound/volume=' + v);
      soundWidget.setVolume(v);
    });

    $('#sound-control #volume #plus').live ('click', function(){
      var v=volume + 5;
      $.get('cgi-bin/set?/sound/volume=' + v);
      soundWidget.setVolume(v);
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
