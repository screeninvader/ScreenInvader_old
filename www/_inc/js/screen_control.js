function ScreenControl () {
  this.init = function() {
    /*$('#screen-control #inputs #input-select').live ('change', function(){
      var selectedInput = $('#switch-control #inputs #input-select option:selected').text();
      $.get('cgi-bin/switch/setInput?' + selectedInput);
    })*/
  };
  
  this.update = function() {
 /*   $.get('cgi-bin/getInput', function(data) {
      $('select > option:selected').val('data'); 
    });*/
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

