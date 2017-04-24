$(document).ready(function() {
var socket = io();


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

    $("#temperature").html(temperature+"&#176");

    socket.on('HVAC', function(msg){
        if(msg.charAt(0) == "0"){
            $('#heating').attr('src','images/heatingDim.png');
        }
        else{
            $('#heating').attr('src','images/heating.png');
        }

        if(msg.charAt(2) == "0"){
            $('#cooling').attr('src','images/coolingDim.png');
        }
        else{
            $('#cooling').attr('src','images/cooling.png');
        }

        if(msg.charAt(4) == "0"){
            $('#fan').attr('src','images/fanDim.png');
        }
        else{
            $('#fan').attr('src','images/fan.png');
        }
    });

    socket.on('Temp', function(msg){
        $("#temperature").html(msg.trim()+"&#176");
    });

    $("#heating").click(function(){
        if(heating == "0"){
            socket.emit('HVAC', "AC|"+"1"+"|"+cooling+"|"+fan);
        }
        else{
            socket.emit('HVAC', "AC|"+"0"+"|"+cooling+"|"+fan);
        }
    });

    $("#cooling").click(function(){
        if(cooling == "0"){
            socket.emit('HVAC', "AC|"+heating+"|"+"1"+"|"+fan);
        }
        else{
            socket.emit('HVAC', "AC|"+heating+"|"+"0"+"|"+fan);
        }
    }); 

    $("#fan").click(function(){
        if(fan == "0"){
            socket.emit('HVAC', "AC|"+heating+"|"+cooling+"|"+"1");
        }
        else{
            socket.emit('HVAC', "AC|"+heating+"|"+cooling+"|"+"0");
        }
    });  

    $('.no-zoom').bind('touchend', function(e) {
        e.preventDefault();
        $(this).click();
    })
});
