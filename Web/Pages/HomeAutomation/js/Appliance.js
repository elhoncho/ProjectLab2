$(document).ready(function() {
	var socket = io();

    if(appliance == "0"){
        $('#appliance').attr('src','images/fanDim.png');
    }
    else{
        $('#appliance').attr('src','images/fan.png');
    }

    socket.on('Appliance', function(msg){
        if(msg.charAt(0) == "0"){
            $('#appliance').attr('src','images/fanDim.png');
        }
        else{
            $('#appliance').attr('src','images/fan.png');
        }
    });

    $("#appliance").click(function(){
    	if(appliance == "0"){
    		socket.emit('Appliance', "AP|"+"1");
    	}
    	else{
    		socket.emit('Appliance', "AP|"+"0");
    	}
    }); 

    $('.no-zoom').bind('touchend', function(e) {
 		e.preventDefault();
  		$(this).click();
})

});