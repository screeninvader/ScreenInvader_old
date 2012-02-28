
function Lounge(local) {
  this.local = local;
 
  this.init = function() {
    this.prefix = location.href.replace(/\/[^\/]*$/, "") + "/";
  }
}

var lounge = new Lounge(true);

$().ready(function () {
  lounge.init();
  switchWidget.loadInto('#switch_panel');
  soundWidget.loadInto('#sound_panel');
  screenWidget.loadInto('#screen_panel');
  showWidget.loadInto('#show_panel');
});