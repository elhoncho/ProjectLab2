$(document).ready(function() {
	var socket = io();
	var appliance = "0";

    $("#low").click(function(){
	socket.emit('Lighting', "LI|65");
    });

    $("#high").click(function(){
	socket.emit('Lighting', "LI|0");
    }); 

    $('.no-zoom').bind('touchend', function(e) {
 		e.preventDefault();
  		$(this).click();
    });

    $(function(){
        $("#slider").slider({
	    min: 0,
	    max: 65,
	    slide: function(event, ui){
                socket.emit('Lighting', "LI|"+ui.value);
            }
	});
    });

});
