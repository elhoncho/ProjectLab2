$(document).ready(function() {
var socket = io();
var heating = "0";
var cooling = "0";
var fan = "0";

$('#toggle-heating').bootstrapToggle('off');
$('#toggle-cooling').bootstrapToggle('off');
$('#toggle-fan').bootstrapToggle('off');

    $("#heating").click(function(){
    if(heating == "0"){
    heating = "1";
    $('#toggle-heating').bootstrapToggle('on');
    socket.emit('HVAC', "AC|"+heating+"|"+cooling+"|"+fan);
    }
    else{
    heating = "0";
    $('#toggle-heating').bootstrapToggle('off');
    socket.emit('HVAC', "AC|"+heating+"|"+cooling+"|"+fan);
    }
    });

    $("#cooling").click(function(){
    if(cooling == "0"){
    cooling = "1";
    $('#toggle-cooling').bootstrapToggle('on');
    socket.emit('HVAC', "AC|"+heating+"|"+cooling+"|"+fan);
    }
    else{
    cooling = "0";
    $('#toggle-cooling').bootstrapToggle('off');
    socket.emit('HVAC', "AC|"+heating+"|"+cooling+"|"+fan);
    }
    }); 

    $("#fan").click(function(){
    if(fan == "0"){
    fan = "1";
    $('#toggle-fan').bootstrapToggle('on');
    socket.emit('HVAC', "AC|"+heating+"|"+cooling+"|"+fan);
    }
    else{
    fan = "0";
    $('#toggle-fan').bootstrapToggle('off');
    socket.emit('HVAC', "AC|"+heating+"|"+cooling+"|"+fan);
    }
    });  

    $('.no-zoom').bind('touchend', function(e) {
        e.preventDefault();
        $(this).click();
    })

});
