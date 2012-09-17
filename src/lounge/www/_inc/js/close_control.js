function CloseControl () {
  this.init = function() {
    $('#close-control #player').live ('click', function(){
      $.get('cgi-bin/trigger?playerClose');
    })

    $('#close-control #image').live ('click', function(){
      $.get('cgi-bin/trigger?browserClose');
    })

    $('#close-control #animation').live ('click', function(){
      $.get('cgi-bin/trigger?browserClose');
    })

    $('#close-control #pdf').live ('click', function(){
      $.get('cgi-bin/trigger?pdfClose');
    })

    $('#close-control #browser').live ('click', function(){
      $.get('cgi-bin/trigger?browserClose');
    })

    $('#close-control #all').live ('click', function(){
      $.get('cgi-bin/trigger?playerClose');
      $.get('cgi-bin/trigger?browserClose');
      $.get('cgi-bin/trigger?pdfClose');
    })
  };
  
  this.enable = function(id, e) {
    if(e) {
      $(id).removeAttr('disabled');
      $(id).css('border-width', '1px');
      $(id).css('color', '#fff');
    }
    else {
      $(id).attr('disabled', 'disabled');
      $(id).css('border-width', '0px');
      $(id).css('color', '#444');
    }
  }
  this.update = function(ScreenInvader) {
    this.enable('#close-control #player', ScreenInvader.player.active == 'true');
    this.enable('#close-control #image', ScreenInvader.image.active == 'true');
    this.enable('#close-control #animation', ScreenInvader.animation.active == 'true');
    this.enable('#close-control #pdf', ScreenInvader.pdf.active == 'true');
    this.enable('#close-control #browser', ScreenInvader.browser.active == 'true');
    this.enable('#close-control #all', ScreenInvader.player.active == 'true' || ScreenInvader.image.active == 'true' || ScreenInvader.animation.active == 'true' || ScreenInvader.pdf.active == 'true' || ScreenInvader.browser.active == 'true'); 
  };
  
  this.loadInto = function(into) {
    return $.get(lounge.prefix + 'close_control.html', function(data) {
      $(into).prepend(data);
    }, "text");
  };
}

closeWidget = new CloseControl();
closeWidget.init();

