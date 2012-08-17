function FindControl () {
  this.init = function() {
     $('#find-control #find-button').live ('click', function(){
      var terms = $('#find-control #terms-input').val();
      $.get('cgi-bin/share/playshare?' + terms);
    })

    $('#find-control #random-button').live ('click', function(){
      var terms = $('#find-control #terms-input').val();
      $.get('cgi-bin/link/randomshare?' + terms);
    })
  };
  
  this.update = function() {

  };
  
  this.loadInto = function(into) {
    return $.get(lounge.prefix + 'find_control.html', function(data) {
      $(into).prepend(data);
    }, "text");
  };
}

findWidget = new FindControl();
findWidget.init();

