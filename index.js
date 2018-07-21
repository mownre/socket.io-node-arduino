var express = require('express');
var socket = require('socket.io');

var app = express();
var server = app.listen(80);

app.use(express.static('public'));

var io = socket(server);


//Configuração do leitor da porta serial 
var SerialPort = require('serialport');

var port = new SerialPort("COM2", {
    baudRate: 9600
});


//Formatador da mensagem recebida pela porta serial
const Readline = require('@serialport/parser-readline')
const parser = port.pipe(new Readline({
    delimiter: '\r\n'
}))


io.on('connection', function () {
    console.log("Uma conexão foi estabelecida!");
});

parser.on('data', function (data) {
    var values = data.toString;
    values = JSON.stringify(data);
    values = JSON.parse(data);

    var arduinos = [];
    arduinos.push(values);

    console.log(values);

    io.sockets.emit('mensagem', values);
});
