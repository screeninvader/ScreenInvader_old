function ShowControl () {
  this.init = function() {
     $('#show-control #show-button').live ('click', function(){
      var url = $('#show-control #url-input').val();
      $.get('cgi-bin/show?' + url);
    })
  };
  
  this.update = function(ScreenInvader) {
  };
  
  this.loadInto = function(into) {
    return $.get(lounge.prefix + 'show_control.html', function(data) {
      $(into).prepend(data);
    }, "text");
  };
}

showWidget = new ShowControl();
showWidget.init();

