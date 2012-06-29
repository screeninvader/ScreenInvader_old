function ScreenControl () {
  this.screen = true;
  this.beamer = false;

  this.setBeamermode = function(on) {
    if(on.trim() == 'false') {
      this.beamer = false;
      $('#screen-control #beamer-toggle').text("Off");
      $('#screen-control #beamer-toggle').addClass("up")
      $('#screen-control #beamer-toggle').removeClass("down");
    } else if(on.trim() == 'true') {
      this.beamer = true;
      $('#screen-control #beamer-toggle').text("On");
      $('#screen-control #beamer-toggle').addClass("down");
      $('#screen-control #beamer-toggle').removeClass("up");
   }
  };

  this.setScreen = function(on) {
    if(on.trim() == 'true') {
      this.screen = true;
      $('#switch-control #screen-toggle').text("Off");
      $('#switch-control #screen-toggle').addClass("up");
      $('#switch-control #screen-toggle').removeClass("down");
    } else if(on.trim() == 'false') {
      this.screen = false;
      $('#switch-control #screen-toggle').text("On"); 
      $('#switch-control #screen-toggle').addClass("down");
      $('#switch-control #screen-toggle').removeClass("up");
    }
  };

  this.init = function() {
    $('#sound-control #screen-toggle').live ('click', function(){
      $.get('cgi-bin/screen/toggleBlank');
      if(screenWidget.screen)
        screenWidget.setScreen('false');
      else
        screenWidget.setScreen('true');
    })

    $('#screen-control #beamer-toggle').live ('click', function(){
      $.get('cgi-bin/screen/toggleBeamermode');
      if(screenWidget.beamer)
        screenWidget.setBeamermode('false');
      else
        screenWidget.setBeamermode('true');
    })
  };
  
  this.update = function() {
    $.get('cgi-bin/screen/isBlank', function(data) {
      screenWidget.setScreen(data);
    }, 'text');

    $.get('cgi-bin/screen/isBeamermode', function(data) {
      screenWidget.setBeamermode(data);
    }, 'text');
  };
  
  this.loadInto = function(into) {
    return $.get(lounge.prefix + 'screen_control.html', function(data) {
      $(into).prepend(data);
    }, "text");
  };
}

screenWidget = new ScreenControl();
screenWidget.init();
screenWidget.update();
setInterval('screenWidget.update()', 3000);
