// server.js
const express = require('express');
const http = require('http');
const socketIo = require('socket.io');
const redis = require('redis');

const app = express();
const server = http.createServer(app);
const io = socketIo(server);

// Create a Redis client (configure as needed)
const redisClient = redis.createClient();

redisClient.on('error', (err) => {
  console.error('Redis error:', err);
});

// Subscribe to your Redis channel
redisClient.subscribe('SPOT_DATA');

redisClient.on('message', (channel, message) => {
  // Parse the incoming JSON message and broadcast it
  try {
    const data = JSON.parse(message);
    io.emit('redisData', data);
  } catch (err) {
    console.error('Error parsing message:', err);
  }
});

io.on('connection', (socket) => {
  console.log('New client connected');
  socket.on('disconnect', () => console.log('Client disconnected'));
});

const PORT = 4000;
server.listen(PORT, () => {
  console.log(`Server listening on port ${PORT}`);
});
