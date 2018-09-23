const mongoose = require('mongoose');

mongoose.connect('mongodb://localhost/tccdatabase', { useNewUrlParser: true }, function() {
    console.log("Conectado ao banco!");
});
mongoose.Promise = global.Promise;

module.exports = mongoose;