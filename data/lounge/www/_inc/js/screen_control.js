function ScreenControl () {
  this.screen = true;

  this.setScreen = function(on) {
    if(on.trim() == 'true') {
      this.screen = true;
      $('#sound-control #screen-toggle').text("Off");
      $('#sound-control #screen-toggle').addClass("up");
      $('#sound-control #screen-toggle').removeClass("down");
    } else if(on.trim() == 'false') {
      this.screen = false;
      $('#sound-control #screen-toggle').text("On"); 
      $('#sound-control #screen-toggle').addClass("down");
      $('#sound-control #screen-toggle').removeClass("up");
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
