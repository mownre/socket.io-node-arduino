var app = angular
    .module('app', [
        'ui.router'
    ]);

var socket = io.connect('http://localhost');

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

app.controller('SistemaController', function ($scope) {

    socket.on('mensagem', function(data) {
        $scope.values = data;
        console.log($scope.values);
        $scope.$apply();
    });

});