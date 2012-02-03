var xml_get_status_url = "/cgi-bin/getstatus";
var xm_get_input_url = "/cgi-bin/getinput";

var debug = false;

if ( debug ) {
	xml_get_status_url = '_inc/xml-fake/getstatus.xml';
	xml_get_input_url = "_inc/xml-fake/getinput.xml";
}

$(document).ready( function ($) {
	var t = this;
	
	t.xml;
	t.current_volume = 0;
	
	$.ajax({
		type: "GET",
		url: xml_get_status_url,
		dataType: "xml",
		success: function(xml) {
			renderXML ( xml );
			this.xml = xml;
			
			$(xml).find('Volume').each(function(){
				t.current_volume = $(this).find ( 'Val' ).text();
				
			});
		}
	});
	
});

function renderXML (xml ) {

	var power = $(xml).find( "Power" );

	if ( power.text() == "On" ) {
		$('#off-btn').show();
		$('#on-btn').hide();
	} else if ( power.text() == "Off" ) {
		$('#on-btn').show();
		$('#off-btn').hide();
	}
	$('#on-off-info').empty();
	
	var input = $(xml).find( 'Input_Sel' ).text();
	$('#input-name').html(input);
	$('[value="'+input+'"]').attr( 'checked', 'checked' ).addClass('chosen-input');
	
	
	$(xml).find('Volume').each(function(){
		var volume = $(this).find ( 'Val' );
		$('#volume-val').html(volume.text());
	
		var muted = $(this).find ( 'Mute' );
		if ( muted == 'On' ) {
			$('#volume-mute').html('UnMute');
		} else if ( muted == 'Off' ) {
			$('#volume-mute').html('Mute');
		}

		
	});
	
	$(xml).find('Tone').each(function(){
		$(this).find('Bass').each(function(){
			var bass = $(this).find('Val').text();
			$('#bass-val').html( bass );
		});
		$(this).find('Treble').each(function(){
			var treble = $(this).find('Val').text();
			$('#treble-val').html( treble );
		});
	});
	
	
	$('#volume-mute').live( 'click', function() {
		window.location.href = '/cgi-bin/vol?-805';
	});
	
	$('#volume-minus').live ('click', function(){
		var new_vol = parseInt( $('#volume-val').html() ) - 5;
		window.location.href = '/cgi-bin/vol?'+ new_vol;
	});
	
	$('#volume-plus').live ('click', function(){
		var new_vol = parseInt( $('#volume-val').html() ) + 5;
		window.location.href = '/cgi-bin/vol?'+ new_vol;
	});
	
	$('#bass-minus').live ('click', function(){
		var new_vol = parseInt( $('#bass-val').html() ) - 5;
		window.location.href = '/cgi-bin/bass?'+ new_vol;
	});
	
	$('#bass-plus').live ('click', function(){
		var new_vol = parseInt( $('#bass-val').html() ) + 5;
		window.location.href = '/cgi-bin/bass?'+ new_vol;
	});
	
	
	$('#treble-minus').live ('click', function(){
		var new_vol = parseInt( $('#treble-val').html() ) - 5;
		window.location.href = '/cgi-bin/treble?'+ new_vol;
	});
	
	$('#treble-plus').live ('click', function(){
		var new_vol = parseInt( $('#treble-val').html() ) + 5;
		window.location.href = '/cgi-bin/treble?'+ new_vol;
	});
}

/* -805 +165  dezibel range */
