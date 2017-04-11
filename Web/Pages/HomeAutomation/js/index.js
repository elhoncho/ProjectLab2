$(document).ready(function() {
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
});
