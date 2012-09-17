function NavigateControl () {
  this.init = function() {
    $('#navigate-control #start').live ('click', function(){
      $.get('cgi-bin/trigger?browserJumpStart');
    })

    $('#navigate-control #pageup').live ('click', function(){
      $.get('cgi-bin/trigger?browserPageUp');
    })

    $('#navigate-control #scrollup').live ('click', function(){
      $.get('cgi-bin//trigger?browserScrollUp');
    })

    $('#navigate-control #scrolldown').live ('click', function(){
      $.get('cgi-bin//trigger?browserScrollDown');
    })

    $('#navigate-control #pagedown').live ('click', function(){
      $.get('cgi-bin/trigger?browserPageDown');
    })

    $('#navigate-control #end').live ('click', function(){
      $.get('cgi-bin/trigger?browserJumpEnd');
    })

   $('#navigate-control #zoomout').live ('click', function(){
      $.get('cgi-bin/trigger?browserZoomOut');
    })

    $('#navigate-control #zoomin').live ('click', function(){
      $.get('cgi-bin/trigger?browserZoomIn');
    })
  };
  
  this.update = function(ScreenInvader) {
    if(ScreenInvader.pdf.active == "true" || ScreenInvader.browser.active == "true")
      $('#navigate-control').css('display', '');
    else
      $('#navigate-control').css('display', 'none');
  };
  
  this.loadInto = function(into) {
    return $.get(lounge.prefix + 'navigate_control.html', function(data) {
      $(into).prepend(data);
    }, "text");
  };
}

navigateWidget = new NavigateControl();
navigateWidget.init();

