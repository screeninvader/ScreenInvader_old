function SoundControl () { 
  this.volume = -320;
  this.bass = 0;
  this.treble = 0;
  this.mute = false;
  this.instance = this;
  this.sv = ScreenInvader;

  this.setVolume = function(level) {
    sv.sound.volume = parseInt(level);
    $('#sound-control #volume #value').text(level);
  }

  this.setMute = function(on) {
    if (on) {
      text = 'Unmute';
      $('#sound-control #volume #mute').removeClass("up").addClass("down");
    } else {
      text = 'Mute';
      $('#sound-control #volume #mute').addClass("up").removeClass("down");
    }
    $('#sound-control #volume #mute').text(text);
  }

  this.update = function() {
    setVolume(sv.sound.volume);
    setMute(sv.sound.mute);
  };

  this.init = function() {
    $('#sound-control #volume #mute').live( 'click', function() {
      var m=!sv.sound.mute;
      $.get('cgi-bin/set?/sound/mute=' + m);
      soundWidget.setMute(m);
    });

    $('#sound-control #volume #minus').live ('click', function(){
      var v=sv.sound.volume - 5;
      $.get('cgi-bin/set?/sound/volume=' + v);
      soundWidget.setVolume(v);
    });

    $('#sound-control #volume #plus').live ('click', function(){
      var v=sv.sound.volume + 5;
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
