function SoundControl () {
  this.init = function() {
    $('#sound-control #volume .button').live( 'click', this.update);

    $('#sound-control #volume #mute').live( 'click', function() {
      $.get('cgi-bin/sound/mute');
    });

    $('#sound-control #volume #minus').live ('click', function(){
      $.get('cgi-bin/sound/volume-less');
    });

    $('#sound-control #volume #plus').live ('click', function(){
      $.get('cgi-bin/sound/volume-more');
    });

    $('#sound-control #bass #minus').live ('click', function(){
      $.get('cgi-bin/sound/bass-less');
    });

    $('#sound-control #bass #plus').live ('click', function(){
      $.get('cgi-bin/sound/bass-more');
    });

    $('#sound-control #treble #minus').live ('click', function(){
      $.get('cgi-bin/sound/treble-less');
    });

    $('#sound-control #treble #plus').live ('click', function(){
      $.get('cgi-bin/sound/treble-more');
    });
  };
  
  this.update = function() {
    $.get('cgi-bin/getVolume', function(data) {
      $('#sound-control #volume #value').html(data);
    });

    $.get('cgi-bin/getBass', function(data) {
      $('#sound-control #volume #value').html(data);
    });

    $.get('cgi-bin/getTreble', function(data) {
      $('#sound-control #volume #value').html(data);
    });
  };
  
  this.loadInto = function(into) {
    return $.get(lounge.prefix + 'sound_control.html', function(data) {
      $(into).prepend(data);
    }, "text");
  };
}

var soundWidget = new SoundControl();
soundWidget.init();