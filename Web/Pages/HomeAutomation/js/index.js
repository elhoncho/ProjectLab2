var appliance = "0";

var temperature = "70";
var heating = "0";
var cooling = "0";
var fan = "0";
var setTemp = "30";

var lightingLvl = 65;
var lighting = false;

var hvacControl = "AUTO";
var hvacMode = "OFF";
var hvacFanMode = "AUTO";




$(document).ready(function() {
    var socket = io();

    $("#content").load("/HVAC.html");

    $("#One").click(function(){
        $("#content").load("/HVAC.html");
    });

    $("#Two").click(function(){
        $("#content").load("/Appliance.html");
    });

    $("#Three").click(function(){
        $("#content").load("/Lighting.html");
    });
	
    $("#Four").click(function(){
        $("#content").load("/Video.html");
    });

    $('.nav a').on('click', function(){
        $('.navbar-toggle').click()
    });

    socket.on('setTemp', function(msg){
        setTemp = msg;
    });

    socket.on('FireAlarm', function(msg){
        if(msg.charAt(0) == "1"){
            var d = new Date();
            var hours = d.getHours();
            var postfix = "AM";
            
            if(hours > 12){
                hours = hours - 12;
                postfix = "PM";
            }
            if(hours == 12){
                postfix = "PM";
            }

            $("#alerts").append(
                    "<div class=\"alert alert-danger alert-dismissable\">\
                    <a href=\"#\" class=\"close\" data-dismiss=\"alert\" aria-label=\"close\">&times;</a>\
                    <strong>Fire Detected!</strong> A fire was detected at "+hours+":"+d.getMinutes()+" "+postfix+"\
                    </div>"
            );  
        }
    });

    socket.on('Doorbell', function(msg){
        if(msg.charAt(0) == "1"){

            var d = new Date();
            var hours = d.getHours();
            var postfix = "AM";
            
            if(hours > 12){
                hours = hours - 12;
                postfix = "PM";
            }
            if(hours == 12){
                postfix = "PM";
            }

            $("#alerts").append(
                    "<div class=\"alert alert-info alert-dismissable\">\
                    <a href=\"#\" class=\"close\" data-dismiss=\"alert\" aria-label=\"close\">&times;</a>\
                    <strong>Ding Dong!</strong> Someone rang the doorbell at "+hours+":"+d.getMinutes()+" "+postfix+"\
                    </div>"
            );  
        }
    });

    socket.on('Motion', function(msg){
        if(msg.charAt(0) == "1"){
            
            var d = new Date();
            var hours = d.getHours();
            var postfix = "AM";
            
            if(hours > 12){
                hours = hours - 12;
                postfix = "PM";
            }
            if(hours == 12){
                postfix = "PM";
            }

            $("#alerts").append(
                    "<div class=\"alert alert-warning alert-dismissable\">\
                    <a href=\"#\" class=\"close\" data-dismiss=\"alert\" aria-label=\"close\">&times;</a>\
                    <strong>Whats that!</strong> Motion was detected at "+hours+":"+d.getMinutes()+" "+postfix+"\
                    </div>"
            );  
        }
    });

    socket.on('Appliance', function(msg){
        if(msg.charAt(0) == "0"){
            appliance = "0";
        }
        else{
            appliance = "1";
        }
    });

    socket.on('Temp', function(msg){
        temperature = msg.trim();
    });

    socket.on('HVAC', function(msg){
        if(msg.charAt(0) == "0"){
            heating = "0";
        }
        else{
            heating = "1";
        }

        if(msg.charAt(2) == "0"){
            cooling = "0";
        }
        else{
            cooling = "1";
        }

        if(msg.charAt(4) == "0"){
            fan = "0";
        }
        else{
            fan = "1";
        }
    });

    socket.on('Lighting', function(msg){
        if(msg == "OFF"){
            lighting = false;
        }
        else{
            lighting = true;
        }
    });

    socket.on('LightingLvl', function(msg){
        lightingLvl = parseInt(msg,10);
    });

    socket.on('hvacControl', function(msg){
        hvacControl = msg;
        console.log('hvacControl: ' + hvacControl);
    });

    socket.on('hvacMode', function(msg){
        hvacMode = msg;
        console.log('hvacMode: ' + hvacMode);
    });

    socket.on('hvacFanMode', function(msg){
        hvacFanMode = msg;
        console.log('hvacFanMode: ' + hvacFanMode);
    });
});
