const DISPLAY_TEMP = 0;
const DISPLAY_SET_TEMP = 1;
var display = DISPLAY_TEMP;
var displayTempTimer;
var displayTime = 3000;

$(document).ready(function() {
    var socket = io();

    $("[name='hvacMode']").bootstrapSwitch();
    $("[name='hvacControl']").bootstrapSwitch();

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

    //Init temp
    $("#temperature").html(temperature+"&#176");

    if(hvacControl == "MANUAL"){

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
    }
    else{
        socket.on('hvacFanMode', function(msg){
            if(msg == "AUTO"){
                $('#fan').attr('src','images/fanAuto.png');
            }
            else{
                $('#fan').attr('src','images/fan.png');
            }
        });

        socket.on('hvacMode', function(msg){
            if(msg == "HEATING"){
                $('#cooling').attr('src','images/coolingDim.png');
                $('#heating').attr('src','images/heating.png');
            }
            else{
                $('#cooling').attr('src','images/cooling.png');
                $('#heating').attr('src','images/heatingDim.png');
            }
        });

    }

    $("#heating").click(function(){
        if(hvacControl == "MANUAL"){
            if(heating == "0"){
                socket.emit('HVAC', "AC|1|0|"+fan);
            }
            else{
                socket.emit('HVAC', "AC|0|0|"+fan);
            }
        }
        else{
            if(hvacMode == "COOLING" || hvacMode == "OFF"){
                socket.emit('hvacMode', "HEATING");
            }
        }
    });

    $("#cooling").click(function(){
        if(hvacControl == "MANUAL"){
            if(cooling == "0"){
                socket.emit('HVAC', "AC|0|1|"+fan);
            }
            else{
                socket.emit('HVAC', "AC|0|0|"+fan);
            }
        }
        else{
            if(hvacMode == "HEATING" || hvacMode == "OFF"){
                socket.emit('hvacMode', "COOLING");
            }
        }
    }); 

    $("#fan").click(function(){
        if(hvacControl == "MANUAL"){
            if(fan == "0"){
                socket.emit('HVAC', "AC|"+heating+"|"+cooling+"|"+"1");
            }
            else{
                socket.emit('HVAC', "AC|"+heating+"|"+cooling+"|"+"0");
            }
        }
        else{
            if(hvacFanMode == "AUTO"){
                socket.emit('hvacFanMode', "ON");
            }
            else{
                socket.emit('hvacFanMode', "AUTO");
            }
        }
    });  


    $("#tempUp").click(function(){
        switch(display){
            case DISPLAY_TEMP:
                display = DISPLAY_SET_TEMP;
                $("#temperature").html(setTemp+"&#176");
                clearTimeout(displayTempTimer);
                displayTempTimer = setTimeout(displayTemp, displayTime);
            break;
            case DISPLAY_SET_TEMP:
                socket.emit('setTemp', parseInt(setTemp)+1);
                clearTimeout(displayTempTimer);
                displayTempTimer = setTimeout(displayTemp, displayTime);
            break;
        }
    });

    $("#tempDown").click(function(){
        switch(display){
            case DISPLAY_TEMP:
                display = DISPLAY_SET_TEMP;
                $("#temperature").html(setTemp+"&#176");
                clearTimeout(displayTempTimer);
                displayTempTimer = setTimeout(displayTemp, displayTime);
            break;
            case DISPLAY_SET_TEMP:
                socket.emit('setTemp', parseInt(setTemp)-1);
                clearTimeout(displayTempTimer);
                displayTempTimer = setTimeout(displayTemp, displayTime);
            break;
        }
    });

    socket.on('Temp', function(msg){
        if(display == DISPLAY_TEMP){
            $("#temperature").html(msg.trim()+"&#176");
        }
    });

    socket.on('setTemp', function(msg){
        if(display == DISPLAY_SET_TEMP){
            $("#temperature").html(msg+"&#176");
        }
    });

    function displayTemp(){
        display = DISPLAY_TEMP;
        $("#temperature").html(temperature+"&#176");
    }

    //Stop iPhone frpm zooming on doubble tap
    $('.no-zoom').bind('touchend', function(e) {
        e.preventDefault();
        $(this).click();
    });
});
