
var display = DISPLAY_TEMP;
var displayTempTimer;
var displayTime = 3000;

$(document).ready(function() {
    var socket = io();

    $("[name='hvacSystem']").bootstrapSwitch({
        onSwitchChange: function(event, state){
            if(state == false){
                if(hvacSystem == "ON"){
                    socket.emit('hvacSystem', "OFF");
                }
            }
            else{
                if(hvacSystem == "OFF"){
                    socket.emit('hvacSystem', "ON");
                }
            }
            false;
        }
    });
    $("[name='hvacControl']").bootstrapSwitch({
        onSwitchChange: function(event, state){
            if(state == false){
                if(hvacControl == "MANUAL"){
                    socket.emit('hvacControl', "AUTO");
                }
            }
            else{
                if(hvacControl == "AUTO"){
                    socket.emit('hvacControl', "MANUAL");
                }
            }
            false;
        }
    });

    socket.on('hvacSystem', function(msg){
        if(msg == "ON"){
             $("[name='hvacSystem']").bootstrapSwitch('state', true);

             if(hvacControl == "MANUAL"){
                SetManual();
             }
             else{
                SetAuto();
             }
         }
         else{
            $("[name='hvacSystem']").bootstrapSwitch('state', false);
            $('#heating').attr('src','images/heatingDim.png');
            $('#cooling').attr('src','images/coolingDim.png');
            $('#fan').attr('src','images/fanDim.png');
         }
    });

    socket.on('hvacControl', function(msg){
        if(msg == "MANUAL"){
            SetManual();
        }
        else{
            SetAuto();
        }
    });

    socket.on('HVAC', function(msg){
        if(hvacMode == "MANUAL"){
            SetManual();
        }
    });

    //Init temp
    $("#temperature").html(temperature+"&#176");

    socket.on('HVAC', function(msg){
        if(hvacSystem == "ON" && hvacControl == "MANUAL"){
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
        }
    });


    socket.on('hvacFanMode', function(msg){
        if(hvacSystem == "ON" && hvacControl == "AUTO"){
            if(msg == "AUTO"){
                $('#fan').attr('src','images/fanAuto.png');
            }
            else if(msg == "ON"){
                $('#fan').attr('src','images/fan.png');
            }
            else if(msg =="OFF"){
              $('#fan').attr('src','images/fanDim.png');
            }
        }
    });

    socket.on('hvacMode', function(msg){
        if(hvacSystem == "ON" && hvacControl == "AUTO"){
            if(msg == "HEATING"){
                $('#cooling').attr('src','images/coolingDim.png');
                $('#heating').attr('src','images/heating.png');
            }
            else{
                $('#cooling').attr('src','images/cooling.png');
                $('#heating').attr('src','images/heatingDim.png');
            }
        }
    });

    $("#heating").click(function(){
        if(hvacSystem == "ON"){
            if(hvacControl == "MANUAL"){
                if(heating == "0"){
                    socket.emit('HVAC', "AC|1|0|"+fan);
                }
                else{
                    socket.emit('HVAC', "AC|0|0|"+fan);
                }
            }
            else{
                if(hvacMode == "COOLING"){
                    socket.emit('hvacMode', "HEATING");
                }
            }
        }
    });

    $("#cooling").click(function(){
        if(hvacSystem == "ON"){
            if(hvacControl == "MANUAL"){
                if(cooling == "0"){
                    socket.emit('HVAC', "AC|0|1|"+fan);
                }
                else{
                    socket.emit('HVAC', "AC|0|0|"+fan);
                }
            }
            else{
                if(hvacMode == "HEATING"){
                    socket.emit('hvacMode', "COOLING");
                }
            }
        }
    });

    $("#fan").click(function(){
        if(hvacSystem == "ON"){
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
                    console.log("Fan: ON");
                    socket.emit('hvacFanMode', "ON");
                }
                else if(hvacFanMode == "ON"){
                    console.log("Fan: AUTO");
                    socket.emit('hvacFanMode', "AUTO");
                }
            }
        }
    });


    $("#tempUp").click(function(){
        switch(display){
            case DISPLAY_TEMP:
                display = DISPLAY_SET_TEMP;
                $("#temperature").html(setTemp+"&#176");
                clearTimeout(displayTempTimer);
                displayTempTimer = setTimeout(DisplayTemp, displayTime);
            break;
            case DISPLAY_SET_TEMP:
                socket.emit('setTemp', parseInt(setTemp)+1);
                clearTimeout(displayTempTimer);
                displayTempTimer = setTimeout(DisplayTemp, displayTime);
            break;
        }
    });

    $("#tempDown").click(function(){
        switch(display){
            case DISPLAY_TEMP:
                display = DISPLAY_SET_TEMP;
                $("#temperature").html(setTemp+"&#176");
                clearTimeout(displayTempTimer);
                displayTempTimer = setTimeout(DisplayTemp, displayTime);
            break;
            case DISPLAY_SET_TEMP:
                socket.emit('setTemp', parseInt(setTemp)-1);
                clearTimeout(displayTempTimer);
                displayTempTimer = setTimeout(DisplayTemp, displayTime);
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



    //Stop iPhone frpm zooming on doubble tap
    $('.no-zoom').bind('touchend', function(e) {
        e.preventDefault();
        $(this).click();
    });
});

function DisplayTemp(){
    display = DISPLAY_TEMP;
    $("#temperature").html(temperature+"&#176");
}

function SetManual(){
    $("[name='hvacControl']").bootstrapSwitch('state', true);

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
}

function SetAuto(){
    $("[name='hvacControl']").bootstrapSwitch('state', false);

    if(hvacFanMode == "AUTO"){
        $('#fan').attr('src','images/fanAuto.png');
    }
    else{
        $('#fan').attr('src','images/fan.png');
    }

    if(hvacMode == "HEATING"){
        $('#cooling').attr('src','images/coolingDim.png');
        $('#heating').attr('src','images/heating.png');
    }
    else if(hvacMode == "COOLING"){
        $('#cooling').attr('src','images/cooling.png');
        $('#heating').attr('src','images/heatingDim.png');
    }
    else{
        $('#cooling').attr('src','images/coolingDim.png');
        $('#heating').attr('src','images/heatingDim.png');
    }
}
