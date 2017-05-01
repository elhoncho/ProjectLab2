var app = require('express')();
var express = require('express')
var http = require('http').Server(app);
var io = require('socket.io')(http);
var net = require('net');

var appliance = "0";

var temperature = "70";
var heating = "0";
var cooling = "0";
var fan = "0";
var setTemp = "30";

var hvacControl = "AUTO";
var hvacMode = "COOLING";
var hvacSystem = "OFF";
var hvacFanMode = "AUTO";

var lightingLvl = 65;
var lighting = false;

// Keep track of the connected clients
var clients = [];
var hvacMsg = "";


app.use("/audio", express.static(__dirname + '/audio'));
app.use("/css", express.static(__dirname + '/css'));
app.use("/fonts", express.static(__dirname + '/fonts'));
app.use("/images", express.static(__dirname + '/images'));
app.use("/js", express.static(__dirname + '/js'));
app.use(express.static('../Pages/HomeAutomation/'));


app.get('/', function(req, res){
res.sendFile(__dirname + '/index.html');
});


http.listen(3000, function(){
  console.log('listening on *:3000');
});

io.on('connection', function(socket){
	console.log('a user connected');

  //Sync state of newly connected client
  socket.emit('HVAC', heating+"|"+cooling+"|"+fan);
  socket.emit('Appliance', appliance);
  socket.emit('Temp', temperature);
  socket.emit('hvacControl', hvacControl);
  socket.emit('hvacMode', hvacMode);
  socket.emit('hvacSystem', hvacSystem);
  socket.emit('hvacFanMode', hvacFanMode);
  socket.emit('setTemp', setTemp);
  
  if(lighting == true){
    socket.emit('Lighting', "ON");
  }
  else{
    socket.emit('Lighting', "OFF");
  }
  
  socket.emit('LightingLvl', lightingLvl);
  
  socket.on('setTemp', function(msg){
    setTemp = msg;
    console.log('setTemp: ' + hvacControl);
    io.sockets.emit('setTemp', setTemp);
  });

  socket.on('hvacControl', function(msg){
    hvacControl = msg;
    console.log('hvacControl: ' + hvacControl);
    io.sockets.emit('hvacControl', hvacControl);
  });

  socket.on('hvacMode', function(msg){
    hvacMode = msg;
    console.log('hvacMode: ' + hvacMode);
    io.sockets.emit('hvacMode', hvacMode);
  });

  socket.on('hvacSystem', function(msg){
    hvacSystem = msg;
    console.log('hvacSystem: ' + hvacSystem);
    io.sockets.emit('hvacSystem', hvacSystem);
  });

  socket.on('hvacFanMode', function(msg){
    hvacFanMode = msg;
    console.log('hvacFanMode: ' + hvacFanMode);
    io.sockets.emit('hvacFanMode', hvacFanMode);
  });
	
	socket.on('HVAC', function(msg){
  	console.log('HVAC: ' + msg);

    //Send to the MSP's
  	clients.forEach(function (client) {
    		client.write(msg);
  	});
	});

	socket.on('Appliance', function(msg){
  	console.log('Appliance: ' + msg);
  	clients.forEach(function (client) {
    		client.write(msg);
  	});
	});


	socket.on('Lighting', function(msg){
    	console.log('Lighting: ' + msg);
    	clients.forEach(function (client) {
      		client.write(msg);
	     });
	});

  socket.on('LightingLvl', function(msg){
      console.log('LightingLvl: ' + msg);
      clients.forEach(function (client) {
          client.write(msg);
       });
  });
	
	socket.on('Video', function(msg){
		console.log('Video: ' + msg);
		clients.forEach(function (client) {
      		client.write(msg);
	   });
	});
})



// Start a TCP Server
net.createServer(function (socket) {  
  // Identify this client
  socket.name = socket.remoteAddress + ":" + socket.remotePort 

  // Put this new client in the list
  clients.push(socket);

  // TODO: Turn this into a query for the state of the device
  //socket.write("Welcome " + socket.name + "\n");
  
  //broadcast(socket.name + " joined the chat\n", socket);

  // Handle incoming messages from clients.
  socket.on('data', function (data) {
    inData = data.toString();
    console.log(inData);
    
    if(inData.startsWith("TM")){
      temperature = inData.slice(3);
      io.sockets.emit('Temp', temperature);
    }
    else if(inData.startsWith("FA")){
      io.sockets.emit('FireAlarm', inData.charAt(3));
    }
    else if(inData.startsWith("DB")){
      io.sockets.emit('Doorbell', inData.charAt(3));
    }
    else if(inData.startsWith("MD")){
      io.sockets.emit('Motion', inData.charAt(3));
    }
    else if(inData.startsWith("AC")){
      heating = inData.charAt(3);
      cooling = inData.charAt(5);
      fan = inData.charAt(7);

      io.sockets.emit('HVAC', heating+"|"+cooling+"|"+fan);
    }
    else if(inData.startsWith("AP")){
      appliance = inData.charAt(3);
      io.sockets.emit('Appliance', appliance);
    }
    else if(inData.startsWith("LI")){
      if(inData.slice(3).trim() == "OFF"){
        lighting = false;
        io.sockets.emit('Lighting', "OFF");
      }
      else{
        lightingLvl = parseInt(inData.slice(3),10);
        lighting = true;
        io.sockets.emit('Lighting', "ON");
        io.sockets.emit('LightingLvl', lightingLvl);
      }
    }
  });

  // Remove the client from the list when it leaves
  socket.on('end', function () {
    clients.splice(clients.indexOf(socket), 1);
  });

  socket.on("error",  function(exception){
    console.log("Error: "+exception.message);
    if(exception.message == "read ECONNRESET"){
      clients.splice(clients.indexOf(socket), 1);
    }
  });


}).listen(5000);
