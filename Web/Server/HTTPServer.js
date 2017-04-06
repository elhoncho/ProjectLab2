var express = require('express')
var app = express()
var socket = require('socket.io-client')('http://localhost');

app.use(express.static('../Pages/DemoPage/'))


app.listen(80, function () {
  console.log('Server is listening on port 80!')
})

socket.on('connection', function(socket){
  console.log('a user connected');
});