
function Lounge(local) {
  this.local = local;
 
  this.init = function() {
    this.prefix = location.href.replace(/\/[^\/]*$/, "") + "/";
  }
}

var lounge = new Lounge(true);

function up() {
   $(this).addClass('up');
   $(this).removeClass('down');

}

function down() {
   $(this).addClass('down');
   $(this).removeClass('up');
}



$().ready(function () {
  lounge.init();
  switchWidget.loadInto('#switch_panel');
  soundWidget.loadInto('#sound_panel');
  screenWidget.loadInto('#screen_panel');
  showWidget.loadInto('#show_panel');
  findWidget.loadInto('#find_panel');
  cleanupWidget.loadInto('#cleanup_panel');
  randomWidget.loadInto('#random_panel');
  
  $('.button').ready(function() {
    $(this).addClass('up');
  });
  $('.button').live('mouseup', function(){
   $(this).addClass('up');
   $(this).removeClass('down');
  }).live('mousedown', function(){
     $(this).addClass('down');
    $(this).removeClass('up');
  });

});
