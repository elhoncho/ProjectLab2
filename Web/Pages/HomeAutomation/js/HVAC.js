$(document).ready(function() {
var socket = io();
var heating = "0";
var cooling = "0";
var fan = "0";

    $("#heating").click(function(){
    if(heating == "0"){
    heating = "1";
    socket.emit('HVAC', "AC|"+heating+"|"+cooling+"|"+fan);
    }
    else{
    heating = "0";
    socket.emit('HVAC', "AC|"+heating+"|"+cooling+"|"+fan);
    }
    });

    $("#cooling").click(function(){
    if(cooling == "0"){
    cooling = "1";
    socket.emit('HVAC', "AC|"+heating+"|"+cooling+"|"+fan);
    }
    else{
    cooling = "0";
    socket.emit('HVAC', "AC|"+heating+"|"+cooling+"|"+fan);
    }
    }); 

    $("#fan").click(function(){
    if(fan == "0"){
    fan = "1";
    socket.emit('HVAC', "AC|"+heating+"|"+cooling+"|"+fan);
    }
    else{
    fan = "0";
    socket.emit('HVAC', "AC|"+heating+"|"+cooling+"|"+fan);
    }
    });  

    $('.no-zoom').bind('touchend', function(e) {
 e.preventDefault();
  $(this).click();
})

});
