/* create the empty screen_invader json object */
var screen_invader = {};

var get_url = "http://10.20.30.51/admin/cgi-bin/get?/."; //the url that we get the whole screeninvader json from
var post_url = "http://10.20.30.51/admin/cgi-bin/save"; //the url to post all the stuff to

var form_submit_errors = 0; //this is used to display different error messages the first time and afterwards


function get_screen_invader(callback) {
  var client = new XMLHttpRequest();
  client.open("GET", get_url)
  client.onreadystatechange = function() { /* do something */ alert('statechange')}
  client.send()
    /*
  $.ajax({
    url: get_url,
    dataType: 'json',
    success: function(data) {
      console.log('data =');
      console.log(data);
      screen_invader = data;
      if(typeof callback === 'function') {
        callback();
      }
    }
  });
    */
  jQuery.getJSON(get_url, function(data) {
    console.log('data =');
    console.log(data);
    screen_invader = data;
    if(typeof callback === 'function') {
      callback();
    }
  }).error(function(err) {
    console.log('error when httprequesting json.');
  });
}

get_screen_invader(function() {
  jQuery(document).ready(function($) {
    if(screen_invader) {
      //set hostname inputfield value
      if(screen_invader.hostname) {
        $('#hostname').val(screen_invader.hostname);
        if(screen_invader.hostname !== $('.back-link').attr('href') ) {
          console.log('back link not equal, setting to:');
          console.log(screen_invader.hostname);
          $('.back-link').attr('href', screen_invader.hostname);
        }
      }
      
      //show or hide the wifi screen_invader
      if(screen_invader.connection) {
        if(screen_invader.connection.value === 'Ethernet') {
          $('.connection-hidden').hide();
          $('#connection-ethernet').prop('checked', true);
        }else if(screen_invader.connection.value === 'Wifi') {
          $('.connection-hidden').show();
          $('#connection-wifi').prop('checked', true); 
        }
      }
      if(screen_invader.wifi && screen_invader.wifi.encryption) {
        $('.encryption-option').each(function() {
          if( $(this).val() === screen_invader.wifi.encryption.value ) {
            $(this).prop('selected', true);
          }
        });
        
        //enable or disable the password input depending on encryption
        if(screen_invader.wifi.encryption.value === 'None') {
          $('#wifi-pass').addClass('disabled-input');
        }else{
          $('#wifi-pass').removeClass('disabled-input');
        }
      }
      if(screen_invader.ipconf && screen_invader.ipconf.mode) {
        //enable or disable inputs in the ip screen_invader depending on dhcp or manual screen_invader chosen
        if(screen_invader.ipconf.mode.value === 'Manual' ) {
          $('#ip-address').removeClass('disabled-input');
          $('#ip-netmask').removeClass('disabled-input');
          $('#ip-gateway').removeClass('disabled-input');
        }else if(screen_invader.ipconf.mode.value === 'DHCP') {
          $('#ip-address').addClass('disabled-input').val('');
          $('#ip-netmask').addClass('disabled-input').val('');
          $('#ip-gateway').addClass('disabled-input').val(''); 
        }
      
        $('#ip-mode-select option').each(function() {
          if($(this).val()=== screen_invader.ipconf.mode.value) {
            $(this).prop('selected', true);
          }
        });
      }
      if(screen_invader.display) {
        
        if(screen_invader.display.adapter && screen_invader.display.adapter.choices) {
          //populate the adapter html with adapters from the screen_invader object
          var adapter_html = '';
          for(var key in screen_invader.display.adapter.choices) {
            var selected = '';
            if(screen_invader.display.adapter.choices[key] === screen_invader.display.adapter.value) {
              selected = " selected";
            }
            
            adapter_html += '<option class="display-adapter-option" value="'+key+'"'+selected+'>'+screen_invader.display.adapter.choices[key]+'</option>';
            
          }
          $('#display-adapter').html(adapter_html);
        }
        
        if(screen_invader.display.resolution && screen_invader.display.resolution.choices) {
          //populate the resolution html with resolutions from the screen_invader object
          var resolution_html = '';
          for(var key in screen_invader.display.resolution.choices) {
            
            var selected = '';
            if(screen_invader.display.resolution.choices[key] === screen_invader.display.resolution.value) {
              selected = " selected";
            }
            
            resolution_html += '<option class="display-resolution-option" value="'+key+'"'+selected+'>'+screen_invader.display.resolution.choices[key]+'</option>';
          }
          $('#display-resolution').html(resolution_html);
        }
      }
      if(screen_invader.storage.disks && screen_invader.storage.disks.choices) {
        
        var storage_html = '';
        for(var key in screen_invader.storage.disks.choices) {
          
          var selected = '';
          if(screen_invader.storage.disks.values[key] === 'true' || screen_invader.storage.disks.values[key] === true) {
            selected = ' checked';
          }
          
          storage_html += '<li value="'+key+'"><label>'+screen_invader.storage.disks.choices[key]+'</label><input class="storage-option" type="checkbox"'+selected+' /></li>';
        }
        $('#storage-ul').html(storage_html);
      }
    }
  });
});


jQuery(document).ready(function($) {
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
  $('#admin-form').live('submit', submit_form);
  $('#submit').live('click', submit_form);
  
  
  function submit_form(e) {
    if(e.preventDefault) {
      e.preventDefault();
    }
    if(!screen_invader || is_object_empty(screen_invader) ) {
      if(form_submit_errors <= 0 ) {
        alert('the http get request from the server either failed or is still pending. please wait for a few seconds and retry.');
        form_submit_errors++;
      }else{
        alert('seems as if there is something wrong on your end. are you connected to the wifi? is the screeninvader server running? if you can answer both of those questions with yes then there is something very fishy going on...');
      }
      return false;
    }
    
    screen_invader.hostname = $('#hostname').val();
    
    screen_invader.connection.value = $('.connection-radio:checked').val();
    
    screen_invader.wifi.encryption.value = $('.encryption-option:selected').val();
    
    screen_invader.wifi.essid = $('#wifi-essid').val();
    screen_invader.wifi.password = $('#wifi-pass').val();
    
    screen_invader.ipconf.address = $('#ip-address').val();
    screen_invader.ipconf.netmask = $('#ip-netmask').val();
    screen_invader.ipconf.gateway = $('#ip-gateway').val();
    
    screen_invader.ipconf.mode.value = $('.ip-mode-option:selected').val();
    
    
    screen_invader.display.adapter.value = $('.display-adapter-option:selected').val();
    screen_invader.display.resolution.value = $('.display-resolution-option:selected').val();
    
    $('.storage-option').each(function(i) {
      screen_invader.storage.disks.values[i] = $(this).prop('checked');
    });
    
    $.post(post_url, screen_invader,
      function(data){
        console.log("data ="); 
        console.log(data);
      },
    "json");
    
    return false;
  }
});


function is_object_empty(ob){
   for(var i in ob){ return false; }
   return true;
}
