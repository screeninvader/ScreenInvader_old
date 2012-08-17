function CleanupControl () {
  
  this.init = function() {
    $('#cleanup-control #cleanup-button').live ('click', function(){
      $.get('cgi-bin/clean');
    })
  };
  
  this.update = function() {
  };
  
  this.loadInto = function(into) {
    return $.get(lounge.prefix + 'cleanup_control.html', function(data) {
      $(into).prepend(data);
    }, "text");
  };
}

cleanupWidget = new CleanupControl();
cleanupWidget.init();
