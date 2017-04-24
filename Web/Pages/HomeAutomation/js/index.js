var appliance = "0";

var temperature = "70";
var heating = "0";
var cooling = "0";
var fan = "0";

var lightingLvl = 65;
var lighting = false;




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
});
