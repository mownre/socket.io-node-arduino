const express = require('express');
const bcrypt = require('bcrypt');
const User = require('../model/User')
const router = express.Router();

router.post('/register', async function(req, res) {
    const { matricula }= req.body;
    try {
        if(await User.findOne({matricula})) {
           return res.status(400).send({error: 'Usuario já existe'}) 
        }


        const user = await User.create(req.body);
        user.senha = undefined;
        
        return res.send({user})


    } catch (err){
        return res.status(400).send({error: 'Registro falhou'});
    }
});

router.post('/autenticate', async function(req, res) {
    const { matricula, senha } = req.body;
    console.log(matricula)
    console.log(senha)
    try {
        const user = await User.findOne({ matricula }, {_id:0, nome: 1});
        console.log(user);

        if(!user) {
            return res.status(400).send({error: "Usuario não encontrado"});
        }

        if(!await bcrypt.compare(senha, user.senha)) {,
            return res.status(400).send({error: "Senha inválida"});
        }
        return res.send({user}).status(200);

    } catch (error) {
        return res.status(400).send({error});
    }



    

});

module.exports = app => app.use('/auth', router);