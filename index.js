/* var express = require('express');
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
}));


io.on('connection', function () {
    console.log("Uma conexão foi estabelecida!");
});

parser.on('data', function (data) {
    console.log(data);
    var values = data.toString;
    values = JSON.stringify(data);
    values = JSON.parse(data);

    var arduinos = [];
    arduinos.push(values);

    console.log(values);

    io.sockets.emit('mensagem', values); 
});
*/
var express = require('express');
var socket = require('socket.io');
var tradutor = require('./util/tradutor')

var app = express();
var server = app.listen(8080, function() {
    console.log("Servidor criado!");
});

var io = socket(server);


//Configuração do leitor da porta serial 
var SerialPort = require('serialport');

var port = new SerialPort("COM2", {
    baudRate: 9600
});

const Readline = require('@serialport/parser-readline')
const parser = port.pipe(new Readline({
    delimiter: '\r\n'
}));


io.on('connection', function () {
    console.log("Uma conexão foi estabelecida!");
});

parser.on('data', function (data) {
   // var string1 = new Buffer(Buffer.from(data, 'utf-8').toString('hex'), 'hex');
    //console.log(string1);
    console.log("Chegou algo:");
    var quadroFinal = tradutor.traduzirQuadro(data);
    console.log(quadroFinal);
    /* var values = data.toString;
    values = JSON.stringify(data);
    values = JSON.parse(data);

    var arduinos = [];
    arduinos.push(values);

    console.log(values);

    io.sockets.emit('mensagem', values);  */
});