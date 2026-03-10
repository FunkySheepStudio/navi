const express = require('express');
const http = require('http');
const app = express();
const path = require('path');
const server = http.createServer(app);  
server.setTimeout(0)
app.use(express.json({limit: '1mb'}))

require('./modules/index')(app)

app.use(express.static(path.join(__dirname, '../docs')))
app.use('*', (req, res) => {
    res.sendFile(path.join(__dirname, '../docs/index.html'));
})

server.listen(80, () => {
    console.log('listening on *:80');
});