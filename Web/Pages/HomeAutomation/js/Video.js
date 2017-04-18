$(document).ready(function() {
	var socket = io();
	
    $('.no-zoom').bind('touchend', function(e) {
 		e.preventDefault();
  		$(this).click();
})

});