function get_settings(callback) {
  //set this url to the url that supplies the json settings object
  /*
  var url = '/';
  var xmlHttp = new XMLHttpRequest();
  xmlHttp.open( "GET", url, false );
  xmlHttp.send( null );
  xmlHttp.onload(loaded);
  */
  var settings = {
    "hostname": "hostname",
    "connection": {
      "choices": ["Wifi", "Ethernet"],
      "value": "Wifi"
    },
    "wifi": { 
      "encryption": {
        "choices" : [ "None", "WEP", "WPA-PSK" ],
        "value": "WPA-PSK"
      },
      "essid": "",
      "password": ""
    },
    "ipconf": { 
      "mode": {
        "choices" : [ "DHCP", "Manual" ],
        "value": "DHCP"
      },
      "address": "",
      "netmask": "255.255.255.0",
      "gateway": ""
    },
    "display": { 
      "adapter": {
        "choices" : [ "VGA-0", "LCD" ],
        "value": "VGA-0"
      },
      "resolution": {
        "choices" : [ "800x600", "1024x768" ],
        "value": "1024x768"
      },
    },
    "storage": { 
      "disks": {
        "choices" : [ "Digital Eastern - 123457689 /dev/sda1", "Maxgate - 13377331 /dev/sdb1" ],
        "values": [ "true", "true" ],
      }
    }
  };

  //loaded has to be redefined once the httprequest above actually does load the json object.
  loaded(settings, callback);
}

//once the http request works, settings shouldnt be passed into this anymore
function loaded(settings, callback) {
  console.log('loaded');
  if(typeof callback === 'function') {
    callback(settings);
  }
}


get_settings(function(settings) {
  jQuery(document).ready(function($) {

    set_default_values(settings);
        
    //disable disabled text fields
    $('.disabled-input').live('focus', function() {
      $(this).blur();
    });
    
    //events
    $('.connection-radio').live('change', function(e) {
      var target = $(e.target);
      if(target.attr('id') === 'connection-wifi') {
        $('.connection-hidden').show();
      }else{
        $('.connection-hidden').hide();
      }
    });
    
    $('#encryption-select').live('change',function(e) {
      if($(this).val() === 'None') {
        $('#wifi-pass').addClass('disabled-input').val('');
      }else{
        $('#wifi-pass').removeClass('disabled-input');
      }
    });
    
    $('#ip-mode-select').live('change',function(e) {
      if($(this).val() === 'DHCP') {
        $('#ip-address').addClass('disabled-input').val('');
        $('#ip-netmask').addClass('disabled-input').val('');
        $('#ip-gateway').addClass('disabled-input').val(''); 
      }else{
        $('#ip-address').removeClass('disabled-input');
        $('#ip-netmask').removeClass('disabled-input');
        $('#ip-gateway').removeClass('disabled-input');
      }
    });
    
    //form submit - jsonify and send to server
    $('#admin-form').live('submit',function(e) {
      if(e.preventDefault) {
        e.preventDefault();
      }
      
      settings.hostname = $('#hostname').val();
      
      settings.connection.value = $('.connection-radio:checked').val();
      
      settings.wifi.encryption.value = $('.encryption-option:selected').val();
      
      settings.wifi.essid = $('#wifi-essid').val();
      settings.wifi.password = $('#wifi-pass').val();
      
      settings.ipconf.address = $('#ip-address').val();
      settings.ipconf.netmask = $('#ip-netmask').val();
      settings.ipconf.gateway = $('#ip-gateway').val();
      
      settings.ipconf.mode.value = $('.ip-mode-option:selected').val();
      
      
      settings.display.adapter.value = $('.display-adapter-option:selected').val();
      settings.display.resolution.value = $('.display-resolution-option:selected').val();
      
      $('.storage-option').each(function(i) {
        settings.storage.disks.values[i] = $(this).prop('checked');
      });
      
    });
    
    
    function set_default_values(settings) {
      //set hostname inputfield value
      if(settings.hostname) {
        $('#hostname').val(settings.hostname);
      }
      
      //show or hide the wifi settings
      if(settings.connection.value === 'Ethernet') {
        $('.connection-hidden').hide();
        $('#connection-ethernet').prop('checked', true);
      }else if(settings.connection.value === 'Wifi') {
        $('.connection-hidden').show();
        $('#connection-wifi').prop('checked', true); 
      }
      
      $('.encryption-option').each(function() {
        if( $(this).val() === settings.wifi.encryption.value ) {
          $(this).prop('selected', true);
        }
      });
      
      //enable or disable the password input depending on encryption
      if(settings.wifi.encryption.value === 'None') {
        $('#wifi-pass').addClass('disabled-input');
      }else{
        $('#wifi-pass').removeClass('disabled-input');
      }
      
      //enable or disable inputs in the ip settings depending on dhcp or manual settings chosen
      if(settings.ipconf.mode.value === 'Manual' ) {
        $('#ip-address').removeClass('disabled-input');
        $('#ip-netmask').removeClass('disabled-input');
        $('#ip-gateway').removeClass('disabled-input');
      }else if(settings.ipconf.mode.value === 'DHCP') {
        $('#ip-address').addClass('disabled-input').val('');
        $('#ip-netmask').addClass('disabled-input').val('');
        $('#ip-gateway').addClass('disabled-input').val(''); 
      }
      
      $('#ip-mode-select option').each(function() {
        if($(this).val()=== settings.ipconf.mode.value) {
          $(this).prop('selected', true);
        }
      });
      
      //populate the adapter html with adapters from the settings object
      var adapter_html = '';
      for(var key in settings.display.adapter.choices) {
        var selected = '';
        if(settings.display.adapter.choices[key] === settings.display.adapter.value) {
          selected = " selected";
        }
        
        adapter_html += '<option class="display-adapter-option" value="'+key+'"'+selected+'>'+settings.display.adapter.choices[key]+'</option>';
        
      }
      $('#display-adapter').html(adapter_html);
      
      //populate the resolution html with resolutions from the settings object
      var resolution_html = '';
      for(var key in settings.display.resolution.choices) {
        
        var selected = '';
        if(settings.display.resolution.choices[key] === settings.display.resolution.value) {
          selected = " selected";
        }
        
        resolution_html += '<option class="display-resolution-option" value="'+key+'"'+selected+'>'+settings.display.resolution.choices[key]+'</option>';
      }
      $('#display-resolution').html(resolution_html);
      
      
      var storage_html = '';
      for(var key in settings.storage.disks.choices) {
        
        var selected = '';
        if(settings.storage.disks.values[key] === 'true' || settings.storage.disks.values[key] === true) {
          selected = ' checked';
        }
        
        storage_html += '<li value="'+key+'"><label>'+settings.storage.disks.choices[key]+'</label><input class="storage-option" type="checkbox"'+selected+' /></li>';
      }
      $('#storage-ul').html(storage_html);
    }
  });
});
