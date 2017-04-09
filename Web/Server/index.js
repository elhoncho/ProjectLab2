
var app = require('express')();
var express = require('express')
var http = require('http').Server(app);
var io = require('socket.io')(http);
var net = require('net');

// Keep track of the connected clients
var clients = [];
var hvacMsg = "";


app.use("/audio", express.static(__dirname + '/audio'));
app.use("/css", express.static(__dirname + '/css'));
app.use("/fonts", express.static(__dirname + '/fonts'));
app.use("/images", express.static(__dirname + '/images'));
app.use("/js", express.static(__dirname + '/js'));
app.use(express.static('../Pages/HVAC/'));


app.get('/', function(req, res){
res.sendFile(__dirname + '/index.html');
});


http.listen(3000, function(){
  console.log('listening on *:3000');
});

io.on('connection', function(socket){
	console.log('a user connected');	
	
	socket.on('HVAC', function(msg){
    	console.log('message: ' + msg);
    	clients.forEach(function (client) {
      		client.write(msg);
    	});
  	});

	socket.on('Appliance', function(msg){
    	console.log('message: ' + msg);
    	clients.forEach(function (client) {
      		client.write(msg);
    	});
  	});


	socket.on('Lighting', function(msg){
    	console.log('message: ' + msg);
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
    console.log(data.toString());
    //broadcast(socket.name + "> " + data, socket);
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
