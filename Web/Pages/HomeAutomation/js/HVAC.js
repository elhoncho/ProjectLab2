$(document).ready(function() {
var socket = io();
var heating = "0";
var cooling = "0";
var fan = "0";

    if(heating == "0"){
        $('#heating').attr('src','images/heatingDim.png');
    }
    else{
        $('#heating').attr('src','images/heating.png');
    }

    if(cooling == "0"){
        $('#cooling').attr('src','images/coolingDim.png');
    }
    else{
        $('#cooling').attr('src','images/cooling.png');
    }

    if(fan == "0"){
        $('#fan').attr('src','images/fanDim.png');
    }
    else{
        $('#fan').attr('src','images/fan.png');
    }

    socket.on('HVAC', function(msg){
        if(msg.charAt(0) == "0"){
            heating = "0";
            $('#heating').attr('src','images/heatingDim.png');
        }
        else{
            heating = "1";
            $('#heating').attr('src','images/heating.png');
        }

        if(msg.charAt(2) == "0"){
            cooling = "0";
            $('#cooling').attr('src','images/coolingDim.png');
        }
        else{
            cooling = "1";
            $('#cooling').attr('src','images/cooling.png');
        }

        if(msg.charAt(4) == "0"){
            fan = "0";
            $('#fan').attr('src','images/fanDim.png');
        }
        else{
            fan = "1";
            $('#fan').attr('src','images/fan.png');
        }
    });

    socket.on('Temp', function(msg){
        $("#temperature").html(msg.trim()+"&#176");
    });

    $("#heating").click(function(){
        if(heating == "0"){
            heating = "1";
            $('#heating').attr('src','images/heating.png');
            socket.emit('HVAC', "AC|"+heating+"|"+cooling+"|"+fan);
        }
        else{
            heating = "0";
            $('#heating').attr('src','images/heatingDim.png');
            socket.emit('HVAC', "AC|"+heating+"|"+cooling+"|"+fan);
        }
    });

    $("#cooling").click(function(){
        if(cooling == "0"){
            cooling = "1";
            $('#cooling').attr('src','images/cooling.png');
            socket.emit('HVAC', "AC|"+heating+"|"+cooling+"|"+fan);
        }
        else{
            cooling = "0";
            $('#cooling').attr('src','images/coolingDim.png');
            socket.emit('HVAC', "AC|"+heating+"|"+cooling+"|"+fan);
        }
    }); 

    $("#fan").click(function(){
        if(fan == "0"){
            fan = "1";
            $('#fan').attr('src','images/fan.png');
            socket.emit('HVAC', "AC|"+heating+"|"+cooling+"|"+fan);
        }
        else{
            fan = "0";
            $('#fan').attr('src','images/fanDim.png');
            socket.emit('HVAC', "AC|"+heating+"|"+cooling+"|"+fan);
        }
    });  

    $('.no-zoom').bind('touchend', function(e) {
        e.preventDefault();
        $(this).click();
    })
});
