var app = angular
    .module('app', [
        'ui.router'
    ]);

app.config(function ($stateProvider, $urlRouterProvider, $locationProvider) {

    $urlRouterProvider.otherwise('/');

    $stateProvider
        .state('home', {
            url: '/',
            template: ''
        })
        .state('sobre', {
            url: '/sobre',
            templateUrl: 'pages/sobre.html'
        })
        .state('sistema', {
            url: '/sistema',
            templateUrl: 'pages/sistema.html',
            controller: 'SistemaController'
        });


    // Utilizando o HTML5 History API
    // $locationProvider.html5Mode(true);
});


var socket = io.connect('http://localhost:8080');
console.log("Tentando conexão");


app.controller('SistemaController', function ($scope) {
    var salas = [];
   /*  salas.push({
        numSala: 8,
        medicoes: {
            temperatura: 23,
            umidade: 77,
            presenca: 1,
            lum: 233
        }
    }); */

    socket.on('quadro', function (data) {
        
        var contem = false;
        for(var i = 0; i < salas.length; i++) {
            if(salas[i].numSala == data.numSala) {
                contem = true;
            }
        }
        

        if (!contem) { //Se essa sala ainda não tiver sido registrada...
            salas.push({
                numSala: data.numSala, //Registre-a sem valores em suas medicoes
                medicoes: {
                    temperatura: null,
                    umidade: null,
                    presenca: null,
                    lum: null
                }
            });
        }
        
        var resultado = salas.filter(sala => {
            return sala.numSala === data.numSala;
        });

        resultado[0].medicoes[data.sensor] = data.medicao;

        salas.forEach(sala => {
            if(sala.numSala == resultado.numSala) {
                salas[salas.indexOf(sala)] = resultado;
            }
        });


        $scope.salas = salas;
        $scope.$apply();
    });

});