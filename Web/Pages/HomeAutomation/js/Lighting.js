$(document).ready(function() {
	var socket = io();

    $('#slider').slider({value:65-lightingLvl});
        socket.on('Lighting', function(msg){
    });

    socket.on('LightingLvl', function(msg){
        $('#slider').slider('value', 65-msg);
    });

    $("#off").click(function(){
        socket.emit('Lighting', "LI|OFF");
    });

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
    	    change: function(event, ui){
                if(event.originalEvent){
                    socket.emit('Lighting', "LI|"+(65-ui.value));
                }
            }   
	   });
    });
});
