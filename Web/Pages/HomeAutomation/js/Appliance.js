$(document).ready(function() {
	var socket = io();
	var appliance = "0";

    if(appliance == "0"){
        $('#appliance').attr('src','images/fanDim.png');
    }
    else{
        $('#appliance').attr('src','images/fan.png');
    }

    $("#appliance").click(function(){
    	if(appliance == "0"){
    		appliance = "1";
            $('#appliance').attr('src','images/fan.png');
    		socket.emit('Appliance', "AP|"+appliance);
    	}
    	else{
    		appliance = "0";
            $('#appliance').attr('src','images/fanDim.png');
    		socket.emit('Appliance', "AP|"+appliance);
    	}
    }); 

    $('.no-zoom').bind('touchend', function(e) {
 		e.preventDefault();
  		$(this).click();
})

});