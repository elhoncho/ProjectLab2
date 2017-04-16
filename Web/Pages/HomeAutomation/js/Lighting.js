$(document).ready(function() {
	var socket = io();
	var appliance = "0";
    var lastTxTime = 0;
    var txDelay = 500;
    var sliderUpdate = true;

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
                    socket.emit('Lighting', "LI|"+ui.value);
                    lastTxTime = Date.now();
                    sliderUpdate = false;
                }
            }   
	   });
    });



    window.setInterval(function(){
        if(sliderUpdate){
            socket.emit('Lighting', "LI|"+$("#slider").slider("value"));
            lastTxTime = Date.now();
            sliderUpdate = false;
        }
    }, 500);


});
