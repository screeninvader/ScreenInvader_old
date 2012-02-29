function RandomControl () {
  this.init = function() {
     $('#random-control #random-button').live ('click', function(){
      var terms = $('#random-control #terms-input').val();
      $.get('cgi-bin/link/randomlink?' + terms);
    })
  };
  
  this.update = function() {
  };
  
  this.loadInto = function(into) {
    return $.get(lounge.prefix + 'random_control.html', function(data) {
      $(into).prepend(data);
    }, "text");
  };
}

randomWidget = new RandomControl();
randomWidget.init();

