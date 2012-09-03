function UploadControl () {
  this.init = function() {
     $('#upload-control #show-button').live ('click', function(){
      var url = $('#upload-control #url-input').val();
      $.get('cgi-bin/show?' + url);
    })
  };
  
  this.update = function(ScreenInvader) {
  };
  
  this.loadInto = function(into) {
    return $.get(lounge.prefix + 'upload_control.html', function(data) {
      $(into).prepend(data);
    }, "text");
  };
}

uploadWidget = new UploadControl();
uploadWidget.init();
