
function ScreenControl () {
  this.blank = false;

  this.setScreen = function(on) {
    blank=on;
    if(on) {
      $('#screen-control #screen-toggle').text("Off");
      $('#screen-control #screen-toggle').addClass("up");
      $('#screen-control #screen-toggle').removeClass("down");
    } else {
      $('#screen-control #screen-toggle').text("On"); 
      $('#screen-control #screen-toggle').addClass("down");
      $('#screen-control #screen-toggle').removeClass("up");
    }
  };

  this.init = function() {
    $('#screen-control #screen-toggle').live ('click', function(){
      var b=!blank;
      $.get('cgi-bin/set?/display/blank=' + b);
      screenWidget.setScreen(b);
    })
  };
  
  this.update = function(ScreenInvader) {
    screenWidget.setScreen(ScreenInvader.display.blank == "true" ? true : false);
  };

  this.loadInto = function(into) {
    return $.get(lounge.prefix + 'screen_control.html', function(data) {
      $(into).prepend(data);
    }, "text");
  };
}

screenWidget = new ScreenControl();
screenWidget.init();

