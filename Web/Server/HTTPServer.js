var express = require('express')
var app = express()

app.use(express.static('../Pages/DemoPage/'))


app.listen(80, function () {
  console.log('Server is listening on port 80!')
})
