

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
    //console.log(quadro);
    for(var i = 0; i < quadro.length; i++) {
        if(i % 2 == 1 && (quadro[i] != String.fromCharCode(0x0D) && quadro[i] != String.fromCharCode(0x0A))) {
            string1.push(quadro[i - 1] + quadro[i]);
        }
    }

    console.log(string1);

    var idSala = parseInt(string1[2], 16);;
    var tipoDeSensor = definirTipoDeSensor(string1[3]);
    var valorMedido = parseInt(string1[4], 16);

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
            return "temperatura";
        case SENSOR_LUMINOSIDADE:
            return "luminosidade";
        case SENSOR_PRESENCA:
            return "presenca";
        case SENSOR_UMIDADE:
            return "umidade";
        default:
            return null;
    }
}
