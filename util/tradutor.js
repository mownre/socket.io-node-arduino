

/* Valores fixos */
const INICIO_QUADRO = '7E'
const FIM_QUADRO = '2A';
const KEAP_ALIVE = '40';
const SENSOR_TEMPERATURA = '01';
const SENSOR_LUMINOSIDADE = '02';
const SENSOR_PRESENCA = '03';
const SENSOR_UMIDADE = '09';

exports.traduzirQuadro = function(data) {
    var string1 = []
    
    var quadro = Buffer.from(data, 'utf-8').toString('hex');
    for(var i = 0; i < quadro.length; i++) {
        if(i % 2 == 1) {
            string1.push(quadro[i - 1] + quadro[i]);
        }
    }

    var idSala = parseInt(string1[2], 16);;
    var tipoDeSensor = definirTipoDeSensor(string1[3]);
    var valorMedido = definirValorMedido(string1[4]);//parseInt(string1[4], 16);

    var quadro = {
        numSala: idSala,
        sensor: tipoDeSensor,
        medicao: valorMedido
    }

    return quadro;
}

var definirTipoDeSensor = function(tipoSensor) {
    switch(tipoSensor){
        case SENSOR_TEMPERATURA:
            return "Temperatura";
        case SENSOR_LUMINOSIDADE:
            return "Luminosidade";
        case SENSOR_PRESENCA:
            return "Presenca";
        case SENSOR_UMIDADE:
            return "Umidade";
        default:
            return null;
    }
}
