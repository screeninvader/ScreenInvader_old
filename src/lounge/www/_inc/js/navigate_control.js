function NavigateControl () {
  this.init = function() {
    $('#navigate-control #start').live ('click', function(){
      $.get('cgi-bin/nav/start');
    })

    $('#navigate-control #pageup').live ('click', function(){
      $.get('cgi-bin/nav/pageup');
    })

    $('#navigate-control #scrollup').live ('click', function(){
      $.get('cgi-bin/nav/scrollup');
    })

    $('#navigate-control #scrolldown').live ('click', function(){
      $.get('cgi-bin/nav/scrolldown');
    })

    $('#navigate-control #pagedown').live ('click', function(){
      $.get('cgi-bin/nav/pagedown');
    })

    $('#navigate-control #end').live ('click', function(){
      $.get('cgi-bin/nav/end');
    })

   $('#navigate-control #zoomout').live ('click', function(){
      $.get('cgi-bin/nav/zoomout');
    })

    $('#navigate-control #zoomin').live ('click', function(){
      $.get('cgi-bin/nav/zoomin');
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

