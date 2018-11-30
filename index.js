var socket = require('socket.io');
var bodyParser = require('body-parser');
var Tradutor = require('./util/Tradutor');



var express = require('express');
var app = express();
app.use(express.static('public'));

app.use(bodyParser.json());
app.use(bodyParser.urlencoded({ extended: false}));

require('./controllers/authController')(app);
require('./controllers/userController')(app);

var server = app.listen(8080, function() {
    console.log("Servidor criado!");
});

var io = socket(server);

//Configuração do leitor da porta serial 
var SerialPort = require('serialport');

var port = new SerialPort("COM9", {
    baudRate: 9600
});

const Readline = require('@serialport/parser-readline')
const parser = port.pipe(new Readline({
    delimiter: '*'
}));


io.on('connection', function () {
    console.log("Uma conexão foi estabelecida!");
});

parser.on('data', function (data) {
    console.log(data);
    var quadroFinal = Tradutor.traduzirQuadro(data);
    console.log(quadroFinal);

    io.sockets.emit('quadro', quadroFinal); 

});