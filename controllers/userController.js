const express = require('express');
const User = require('../model/User')
const router = express.Router();



router.get('/', async function(req, res) {
    try {
        var usuarios = await User.find({});
        return res.status(200).send(usuarios);
    } catch (err) {
        return res.status(400).send({error: 'GET Usuarios falhou'});
    }
});


module.exports = app => app.use('/usuarios', router);