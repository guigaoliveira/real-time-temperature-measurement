const path = require('path')
const http     = require('http')
const express  = require('express')
const app      = express();
const httpServer = http.createServer(app)
const mosca    = require('mosca')

const ascoltatore = {
  //using ascoltatore
  type: 'mongo',
  url: 'mongodb://localhost:27017/mqtt',
  pubsubCollection: 'ascoltatori',
  mongo: {}
};
const settings = {
  port: 1883,
  backend: ascoltatore,
  persistence: {
    factory: mosca.persistence.Mongo,
    url: 'mongodb://localhost:27017/mqtt'
  }
};

const broker = new mosca.Server(settings)

broker.on('clientConnected', (client) => {
    console.log('client connected', client.id)
});

// fired when a message is received/js
broker.on('published', (packet, client) => {
  console.log('Published', packet.payload.toString())
})
broker.on('ready', () => console.log('Mosca server is up and running'))

app.use(express.static('static'))

broker.attachHttpServer(httpServer)
httpServer.listen(3000, () => console.log('servidor subiu!'))
