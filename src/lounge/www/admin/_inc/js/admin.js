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
    $.postJSON('/admin/cgi-bin/save', admin.data, function(){
      $.get('/admin/cgi-bin/trigger?apply', function() {
        admin.reload();
      });
    });
  }

  this.refresh = function()  {
    admin.data.display.adapter.value = $('#adapter-choices').val();
    admin.data.display.resolution.value = $('#resolution-choices').val();
    admin.data.network.connection.value = $('#connection-choices').val();
    admin.data.display.resolution.value = $('#resolution-choices').val();
    admin.data.network.connection.value = $('#connection-choices').val();
    admin.data.network.connection.interface = $('#interface-choices').val();
    admin.data.network.mode.value = $('#mode-choices').val();
    admin.data.network.wifi.encryption.value = $('#encryption-choices').val();
    admin.data.network.hostname = $('#hostname').val();
    admin.data.network.wifi.ssid = $('#ssid-value').val();
    admin.data.network.wifi.ssid = $('#passphrase-value').val();
    admin.data.network.address = $('#address').val();
    admin.data.network.gateway = $('#gateway').val();
    admin.data.network.nameserver = $('#nameserver').val();
    admin.data.network.netmask = $('#netmask').val();
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
    this.setVisible('#passphrase', adminData.network.wifi.encryption.value != 'NONE');
    this.setVisible('#ipconf', adminData.network.mode.value == "Manual");

    $('#resolution-choices option').remove();
    $('#connection-choices option').remove();
    $('#mode-choices option').remove();
    $('#encryption-choices option').remove();
    $('#interface-choices option').remove();
    $('#adapter-choices option').remove();

    this.makeOptions('#resolution-choices',adminData.display.resolution);
    this.makeOptions('#connection-choices',adminData.network.connection);
    this.makeOptions('#mode-choices', adminData.network.mode);
    this.makeOptions('#encryption-choices', adminData.network.wifi.encryption);
    this.makeOptions('#adapter-choices', adminData.display.adapter);

    if(adminData.network.connection.value == "Wifi") {
      this.makeOptions('#interface-choices', adminData.network.connection.wireless);
    } else {
      this.makeOptions('#interface-choices', adminData.network.connection.wired);
    }

    $('#hostname').val(adminData.network.hostname);
    $('#ssid-value').val(adminData.network.wifi.ssid);
    $('#passphrase-value').val(adminData.network.wifi.passphrase);
    $('#address').val(adminData.network.address);
    $('#gateway').val(adminData.network.gateway);
    $('#netmask').val(adminData.network.netmask);
    $('#nameserver').val(adminData.network.nameserver);
    admin.data.network.connection.interface = $('#interface-choices').val(); 
    admin.data.network.connection.interface = $('#interface-choices').val();
  };

  this.reload = function() {
    $.get('/admin/cgi-bin/trigger?reload', function () {
      $.get('/admin/cgi-bin/get?/.', function(data) {
        admin.update(data);
      });
    });
  }

  this.init = function() {
    admin.reload();
    $('#adapter-choices').change( function() {
       admin.refresh();
    });

    $('#resolution-choices').change( function() {
      admin.refresh();
    });

    $('#connection-choices').change( function() {
       admin.refresh();
    });

    $('#interface-choices').change( function() {
      admin.refresh();
    });

    $('#mode-choices').change( function() {
       admin.refresh();
    });

    $('#encryption-choices').change( function() {
       admin.refresh();
    });

    $('#hostname').change( function() {
      admin.refresh();
    });

    $('#ssid-value').change( function() {
      admin.refresh();
    });

    $('#passphrase-value').change( function() {
      admin.refresh();
    });

    $('#address').change( function() {
      admin.refresh();
    });

    $('#gateway').change( function() {
      admin.refresh();
    });

    $('#nameserver').change( function() {
      admin.refresh();
    });

    $('#netmask').change( function() {
      admin.refresh();
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
