function ScreenControl () {
  this.screen = true;
  this.sv = ScreenInvader;

  this.setScreen = function(on) {
    sv.display.blank=on;
    if(on) {
      $('#sound-control #screen-toggle').text("Off");
      $('#sound-control #screen-toggle').addClass("up");
      $('#sound-control #screen-toggle').removeClass("down");
    } else if(on.trim() == 'false') {
      $('#sound-control #screen-toggle').text("On"); 
      $('#sound-control #screen-toggle').addClass("down");
      $('#sound-control #screen-toggle').removeClass("up");
    }
  };

  this.init = function() {
    $('#sound-control #screen-toggle').live ('click', function(){
      var b=!sv.display.blank;
      $.get('cgi-bin/set?/display/blank=' + b);
      screenWidget.setScreen(b);
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

