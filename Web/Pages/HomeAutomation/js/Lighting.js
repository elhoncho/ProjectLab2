$(document).ready(function() {
	var socket = io();
    var lastTxTime = 0;
    var txDelay = 250;
    var sliderUpdate = false;

    $('#slider').slider({value:65-lightingLvl});

    socket.on('Lighting', function(msg){
        
    });

    socket.on('LightingLvl', function(msg){
        $('#slider').slider('value', 65-msg);
    });

    $("#off").click(function(){
        if(Date.now() - lastTxTime > txDelay){
            socket.emit('Lighting', "LI|OFF");
            lastTxTime = Date.now();
        }
    });

    $("#low").click(function(){
	   if(Date.now() - lastTxTime > txDelay){
            socket.emit('Lighting', "LI|65");
            lastTxTime = Date.now();
        }
    });

    $("#high").click(function(){
        if(Date.now() - lastTxTime > txDelay){
            socket.emit('Lighting', "LI|0");
            lastTxTime = Date.now();
        }
	
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
                sliderUpdate = true;
                if(Date.now() - lastTxTime > txDelay){
                    socket.emit('Lighting', "LI|"+(65-ui.value));
                    lastTxTime = Date.now();
                    sliderUpdate = false;
                }
            }   
	   });
    });



    window.setInterval(function(){
        if(sliderUpdate){
            socket.emit('Lighting', "LI|"+65-$("#slider").slider("value"));
            lastTxTime = Date.now();
            sliderUpdate = false;
        }
    }, txDelay);


});
