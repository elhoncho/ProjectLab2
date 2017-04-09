$(document).ready(function() {
	var socket = io();
	var appliance = "0";

    $("#appliance").click(function(){
    	if(appliance == "0"){
    		appliance = "1";
    		socket.emit('Appliance', "AP|"+appliance);
    	}
    	else{
    		appliance = "0";
    		socket.emit('Appliance', "AP|"+appliance);
    	}
    }); 

    $('.no-zoom').bind('touchend', function(e) {
 		e.preventDefault();
  		$(this).click();
})

});