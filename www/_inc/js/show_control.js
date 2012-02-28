function ShowControl () {
  this.init = function() {
   /* $('#show-control #inputs #input-select').live ('change', function(){
      var selectedInput = $('#switch-control #inputs #input-select option:selected').text();
      $.get('cgi-bin/switch/setInput?' + selectedInput);
    })*/
  };
  
  this.update = function() {
    /*$.get('cgi-bin/getInput', function(data) {
      $('select > option:selected').val('data'); 
    });*/
  };
  
  this.loadInto = function(into) {
    return $.get(lounge.prefix + 'show_control.html', function(data) {
      $(into).prepend(data);
    }, "text");
  };
}

showWidget = new ShowControl();
showWidget.init();
showWidget.update();

