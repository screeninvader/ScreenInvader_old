function SwitchControl () {
  this.init = function() {
    $('#switch-control #inputs #input-select').live ('change', function(){
      var selectedInput = $('#switch-control * option:selected').each(function() {
        $.get('cgi-bin/switch/setInput?'  + ($(this).index() + 1));
      });
    })
  };
  
  this.update = function(ScreenInvader) {
    $.get('cgi-bin/switch/getInput', function(data) {
      $('#switch-control * option').eq(parseInt(data) - 1).attr('selected', 'selected');
    }, 'text');
  };
  
  this.loadInto = function(into) {
    return $.get(lounge.prefix + 'switch_control.html', function(data) {
      $(into).prepend(data);
    }, "text");
  };
}

switchWidget = new SwitchControl();
switchWidget.init();
