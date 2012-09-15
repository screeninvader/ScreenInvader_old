JSON.stringify = JSON.stringify || function (obj) {
    var t = typeof (obj);
    if (t != "object" || obj === null) {
        // simple data type
        if (t == "string") obj = '"'+obj+'"';
        return String(obj);
    }
    else {
        // recurse array or object
        var n, v, json = [], arr = (obj && obj.constructor == Array);
        for (n in obj) {
            v = obj[n]; t = typeof(v);
            if (t == "string") v = '"'+v+'"';
            else if (t == "object" && v !== null) v = JSON.stringify(v);
            json.push((arr ? "" : '"' + n + '":') + String(v));
        }
        return (arr ? "[" : "{") + String(json) + (arr ? "]" : "}");
    }
};

$.postJSON = function(url, data, callback) {
    return jQuery.ajax({
        'type': 'POST',
        'url': url,
        'contentType': 'application/json',
        'data': JSON.stringify(data),
        'dataType': 'json',
        'success': callback
    });
};

function AdminControl () {
  this.data;

  this.makeOptions = function(id, obj) {
    var value = obj.value;

    $.each(obj.choices, function(i,c) {
      var selected;
      if(value == c) {
        selected="selected";
      }

      $(id).append('<option value="' + c + '" ' + selected + '>' + c + '</option>');
    });
  }

  this.save = function() {
    $.postJSON('/admin/cgi-bin/save', admin.data, function(){});
  }

  this.refresh = function()  {
    admin.update(admin.data);
  } 

  this.setVisible = function(id,v) {
    if(v)
      $(id).css('display', '');
    else
      $(id).css('display', 'none');
  }

  this.update = function(adminData) {
    this.data = adminData;

    this.setVisible('#wifi', adminData.network.connection.value == "Wifi");
    this.setVisible('#passphrase', adminData.network.wifi.encryption.value == 'None');
    this.setVisible('#ipconf', adminData.network.mode.value == "Manual");

    $('#resolution-choices option').remove();
    $('#connection-choices option').remove();
    $('#mode-choices option').remove();
    $('#encryption-choices option').remove();

    this.makeOptions('#resolution-choices',adminData.display.resolution);
    this.makeOptions('#connection-choices',adminData.network.connection);
    this.makeOptions('#mode-choices', adminData.network.mode);
    this.makeOptions('#encryption-choices', adminData.network.wifi.encryption);

    $('#hostname').val(adminData.network.hostname);
    $('#ssid').val(adminData.network.wifi.ssid);
    $('#address').val(adminData.network.address);
    $('#gateway').val(adminData.network.gateway);
    $('#netmask').val(adminData.network.netmask);
  };

  this.init = function() {
    $.get('/admin/cgi-bin/get?/.', function(data) {
      admin.update(data);
    });

    $('#resolution-choices').change( function() {
       admin.data.display.resolution.value = $('#encryption-choices').val();
    });

    $('#connection-choices').change( function() {
       admin.data.network.connection.value = $('#connection-choices').val();
       admin.refresh();
    });

    $('#mode-choices').change( function() {
       admin.data.network.mode.value = $('#mode-choices').val();
       admin.refresh();
    });

    $('#encryption-choices').change( function() {
       admin.data.wifi.encryption.value = $('#encryption-choices').val();
       admin.refresh();
    });

    $('#hostname').change( function() {
       admin.data.network.hostname = $('#hostname').val();
    });

    $('#ssid').change( function() {
       admin.data.network.wifi.ssid = $('#ssid').val();
    });

    $('#address').change( function() {
      admin.data.network.address = $('#address').val();
    });
     $('#gateway').change( function() {
      admin.data.network.gateway = $('#gateway').val();
    });
    $('#netmask').change( function() {
      admin.data.network.netmask = $('#netmask').val();
    });

    $('#save').live ('click', function(){
      admin.save();
    });
  };
}

var admin = new AdminControl();

$().ready(function () {
  admin.init();
});
