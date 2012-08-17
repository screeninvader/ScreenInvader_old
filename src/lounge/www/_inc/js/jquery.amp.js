/***************************
 * 
 * jQuery.amp.js - controller application for the metalablounge
 * 
 * part of github: https://github.com/kallaballa/LoungeControl
 * 
 * */


// the zero volume value is -805. so adding 805 to the volume makes it start by 0
var get_to_zero_volume = 805;

// checking if we are local or deployed
var local = false;
var urlprefix = '/';

var host = document.location.href;
if ( host.indexOf("localhost") != -1 || host.indexOf("file://") != -1 ) {
	local = true;
	urlprefix = '../';
}

// xml urls to get the data from
var xml_get_status_url = "http://10.20.30.51/cgi-bin/getstatus";
var xml_get_input_url = "http://10.20.30.51/cgi-bin/getinput";

// default xml string to test locally
var xmlstats = '<YAMAHA_AV rsp="GET" RC="0"><Main_Zone><Basic_Status><Power_Control><Power>Off</Power><Sleep>Off</Sleep></Power_Control><Volume><Lvl><Val>-185</Val><Exp>1</Exp><Unit>dB</Unit></Lvl><Mute>Off</Mute></Volume><Input><Input_Sel>AUDIO3</Input_Sel><Input_Sel_Item_Info><Param>AUDIO1</Param><RW>RW</RW><Title>AUDIO1</Title><Icon><On>/YamahaRemoteControl/Icons/icon002.png</On><Off></Off></Icon><Src_Name></Src_Name><Src_Number>1</Src_Number></Input_Sel_Item_Info></Input><Surround><Program_Sel><Current><Straight>Off</Straight><Enhancer>On</Enhancer><Sound_Program>7ch Stereo</Sound_Program></Current></Program_Sel><_3D_Cinema_DSP>Auto</_3D_Cinema_DSP><Dialogue_Lift>0</Dialogue_Lift></Surround><Pure_Direct><Mode>Off</Mode></Pure_Direct><Sound_Video><Tone><Bass><Val>50</Val><Exp>1</Exp><Unit>dB</Unit></Bass><Treble><Val>-50</Val><Exp>1</Exp><Unit>dB</Unit></Treble></Tone><Adaptive_DRC>Off</Adaptive_DRC></Sound_Video></Basic_Status></Main_Zone></YAMAHA_AV>';

/******************************************* DOCUMENT.READY **********************************************/
$(document).ready( function ($) {
	var t = this;
	
	// creating default variables for later
	t.xml;
	t.current_volume = 0;
	t.current_db_volume = 0;
	t.volume_to_set = 0;
	t.max_bass = 50;
	t.max_treble = 50;
	
	t.on = false;
	
	t.search_and_play_visible = false;
	t.upload_stream_visible = false;
	
	if ( !local ) {
		$.ajax({
			type: "GET",
			url: xml_get_status_url,
			dataType: "xml",
			success: function(xml) {
				renderXML ( xml, t );
				this.xml = xml;
				
				$(xml).find('Volume').each(function(){
					t.current_volume = $(this).find ( 'Val' ).text();
					
				});
			}
		});
	}else {
			renderXML(xmlstats, t);		
	}
});


/************************ END OF DOCUMENT.READY *****************************************/


/*********************** RENDER XML ****************************************************/
function renderXML (xml, t ) {
	
	// determine if the amp is powered on
	var power = $(xml).find( "Power" );

	//show the button according to the settings
	if ( power.text() == "On" ) {
		$('#meta-logo').removeClass('off').addClass('on');
		
		$('#content').animate({'height': 'toogle', 'opacity': 1 }, 500).removeClass('hidden');
		// hide the "loading" message
		$('#on-off-info').empty();
		t.on = true;
	} else {
		$('#on-off-info').html('Lounge Control is deactivated. Press the logo to activate.');
		t.on = false;
	}
	
	/************* populating default html values ***********/
	var input = $(xml).find( 'Input_Sel' ).text();
	$('#input-name').html(input);
	$('[value="'+input+'"]').attr( 'checked', 'checked' ).addClass('chosen-input');
	
	/************ current volume ************************/
	$(xml).find('Volume').each(function(){
		var volume = $(this).find ( 'Val' );
		$('#volume-val').html( parseInt ( volume.text() ) + get_to_zero_volume);
		$('#db-val').html ( parseInt ( volume.text() ) * 0.1 );
		t.current_db_volume = volume.text();
		
		t.current_volume = volume.text();
		var muted = $(this).find ( 'Mute' );
		if ( muted == 'On' ) {
			$('#volume-mute').html('UnMute');
		} else if ( muted == 'Off' ) {
			$('#volume-mute').html('Mute');
		}
	});
	
	
	/*************** get bass and treble ********************/
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
	
	$('.input-select').live ('click', function() {
		$.get( urlprefix+'cgi-bin/switch?'+$(this).val() );
		
	});
	
	
	blank=true
	//blank or unblank the screen
	$('#screentoggle').live('click',function(){	
  	 	if(blank) {
                  $('#screentoggle').text("OFF").removeClass("green").addClass("red");
		  $.get( urlprefix+'cgi-bin/blank');
		} else {
                  $('#screentoggle').text("ON").removeClass("red").addClass("green");
		  $.get( urlprefix+'cgi-bin/unblank');
		}
		blank=!blank;
		return false;
	});
	
	beamer=false
        $('#beamertoggle').live('click',function(){
		beamer=!beamer
		if(beamer) {
                  $('#beamertoggle').text("ON");
		  $.get( urlprefix+'cgi-bin/beamermode?on' );
		} else {
                  $('#beamertoggle').text("OFF");
                $.get( urlprefix+'cgi-bin/beamermode?off' );
		}
                return false;
        });


	
	
	//turns the amplifier off
	$('#meta-logo').click(function() {
		if ( t.on == false ){
			$.get ( urlprefix+'cgi-bin/on' );
			
			t.on = true;
			$('#content').animate({'height': 'toggle', 'opacity': 1 }, 500, function(){
				$('#meta-logo').removeClass('off').addClass('on');
				$('#on-off-info').html('');
			});
		} else {
			$.get ( urlprefix+'cgi-bin/off' );
			t.on = false;
			$('#content').animate({ 'height': 'toggle', 'opacity': 0 }, 500, function(){
				$(this).hide();
				$('#meta-logo').removeClass('on').addClass('off');
			});
			$('#on-off-info').html('Lounge Control is deactivated. Press the logo to activate.');
		}
	});
	
	/***************************** START OF VOLUME ************************************/
	
	// mute on/off btn
	t.is_muted = false;
	$('#volume-mute').live( 'click', function() {
		if ( !t.is_muted ) {
			$.get(urlprefix+'cgi-bin/muteon');
			t.is_muted = true;
		}else{
			$.get(urlprefix+'cgi-bin/muteoff');
			t.is_muted = false;
		}
	});
	
	//SHHHHH ;)
	$('#volume-minus').live ('click', function(){
		var new_vol = parseInt( $('#volume-val').html() ) - 5;
		var new_db = parseInt( t.current_db_volume ) - 5;
		
		apply_volume ( new_vol, new_db );
	});
	
	//LOOOUDER ;)
	$('#volume-plus').live ('click', function(){
		var new_vol = parseInt( $('#volume-val').html() ) + 5;
		var new_db = parseInt( t.current_db_volume ) + 5;
		
		apply_volume ( new_vol, new_db );
	});
	
	function apply_volume ( new_vol, new_db ) {
		
		$('#volume-val').html( new_vol );
		$('#db-val').html ( new_db * 0.1 );
		
		t.current_volume = new_vol;
		t.current_db_volume = new_db;
		
		setTimeout( function(){post_volume( new_db )}, 500);
		
	}
	
	function post_volume ( new_db ) {
		if ( t.current_db_volume == new_db ) {
			$.get( urlprefix + 'cgi-bin/vol?' + new_db);
		}
	}
	
	/***************************** END OF VOLUME ************************************/
	
	
	/***************************** START OF BASS ************************************/
	
	//adjusts bass minus
	$('#bass-minus').live ('click', function(){
		var new_vol = parseInt( $('#bass-val').html() ) - 5;
		
		if ( new_vol < -t.max_bass ) {
			new_vol = -t.max_bass;
			return;
		}
		
		apply_bass( new_vol );
	});
	
	//adjusts bass plus
	$('#bass-plus').live ('click', function(){
		var new_vol = parseInt( $('#bass-val').html() ) + 5;
		
		if ( new_vol > t.max_bass ) {
			new_vol = t.max_bass;
			return;
		}
		
		apply_bass( new_vol );
	});
	
	function apply_bass ( new_vol ) {
		t.current_bass = new_vol;
		$('#bass-val').html( t.current_bass );
		
		setTimeout( function(){post_bass( new_vol )}, 500);
	}
	
	
	function post_bass ( new_vol ) {
		if ( t.current_bass == new_vol ) {
			$.get( urlprefix + 'cgi-bin/bass?' + new_vol);
		}
	}
	
	
	/***************************** END OF BASS ************************************/
	
	//adjusts treble minus
	$('#treble-minus').live ('click', function(){
		var new_vol = parseInt( $('#treble-val').html() ) - 5;
		if ( new_vol < -t.max_treble ) {
			new_vol = -t.max_treble;
			return;
		}
		apply_treble ( new_vol );
	});
	
	//adjusts treble plus
	$('#treble-plus').live ('click', function(){
		var new_vol = parseInt( $('#treble-val').html() ) + 5;
		if ( new_vol > t.max_treble ) {
			new_vol = t.max_treble;
			return;
		}
		apply_treble( new_vol );
	});
	
	function apply_treble ( new_vol ) {
		t.current_treble = new_vol;
		$('#treble-val').html( t.current_treble );
		
		setTimeout( function(){post_treble( new_vol )}, 500);
	}
	
	function post_treble ( new_vol ){
		if ( t.current_treble == new_vol ) {
			$.get( urlprefix + 'cgi-bin/treble?' + new_vol);
		}
	}
	
	
	// loads a url in the screen, distinguishes between youtube and the rest of the web atm
	$('#browse-button').live ( 'click', function(){
		
		var target = $('#url-to-load').val();
		
		if ( target == "" || !target ) {
			return false;
		}
		var cgi_target = 'browse';
		
		if ( target.indexOf('youtube.com/watch') != -1 || target.indexOf('youtu.be/watch') != -1 ) {
			cgi_target = 'youtube';
		} 
		var realtarget = urlprefix+'cgi-bin/'+cgi_target+'?'+target;

		$.get( realtarget );
	});
	
	$('#clean').live('click', function() {
		$.get( urlprefix + 'cgi-bin/clean' );
	});
	
	/*************** Show/Hide advanced video controls *****************/
	t.video_player_shown = false;
	t.video_is_paused = false;
	
	$('#show-video-player-button').live ('click', function(){
		var gui = $('#video-player-gui');
		gui.animate({
				height: 'toggle'
			}, 100, function(){
				gui.removeClass('hidden');1
			});
			
		if ( t.video_player_shown == false ) {
			
			$(this).html('Hide Videoplayer');
			t.video_player_shown = true;
		}else{
			$(this).html('Show Videoplayer');
			t.video_player_shown = false;
		}
	});
	
	$('.video-pause-play-button').live ('click', function() {
		if ( t.video_is_paused ) {
			t.video_is_paused = false;
			$('#video-pause-button').show();
			$('#video-play-button').hide();
		}else {
			t.video_is_paused = true;
			$('#video-pause-button').hide();
			$('#video-play-button').show();
			$('#video-play-button').html('');
		}
		$.get( urlprefix+'cgi-bin/mplayer/pause');
	});
	
	$('.video-control-button').live ( 'click', function () {
		var cgi_target = $(this).attr('id');
		$.get( urlprefix+'cgi-bin/mplayer/'+cgi_target );
	});
	
	$('#start-video-stream').live ('click', function () {
		$('#video-controls').show();
	});
	
	$('#vid-load-button').live ('click', function(){
		var search_term = $('#vid-load-text').html();
		$.get( urlprefix+'cgi-bin/playlink?'+search_term );
	});
	
	
	
	$('#life-stream-upload-button').live ( 'click', function () {
		if ( t.upload_stream_visible ) {
			$('#life-stream-container').hide();
			t.upload_stream_visible = false;
		} else {
			if ( t.search_and_play_visible ){
				$('#search-and-play-container').hide();
				t.search_and_play_visible = false;
			} 
			$('#life-stream-container').show().removeClass('hidden');
			t.upload_stream_visible = true;
		}
	});
	
	$('#search-and-play-button').live('click', function () {
		if ( t.search_and_play_visible ) {
			$('#search-and-play-container').hide();
			t.search_and_play_visible = false;
		} else {
			if ( t.upload_stream_visible ){
				$('#life-stream-container').hide();
				t.upload_stream_visible = false;
			} 
			$('#search-and-play-container').show().removeClass('hidden');
			t.search_and_play_visible = true;
		}
	});
	
}
/********************************** END OF RENDERXML ******************************************/
