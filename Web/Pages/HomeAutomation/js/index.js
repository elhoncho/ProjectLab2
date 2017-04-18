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
	
    $("#Four").click(function(){
        $("#content").load("/Video.html");
    });

    $('.nav a').on('click', function(){
        $('.navbar-toggle').click()
    });
});
