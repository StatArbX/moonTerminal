const express = require('express');
const { createClient } = require('redis');
const cors = require('cors');

const app = express();
app.use(cors());

const subscriber = createClient({
    url: 'redis://localhost:6379'
});

// Maintain a list of SSE clients
let clients = [];

app.get('/spotDataStream', (req, res) => {
    res.setHeader('Content-Type', 'text/event-stream');
    res.setHeader('Cache-Control', 'no-cache');
    res.setHeader('Connection', 'keep-alive');
    res.flushHeaders();

    // Add client to the list
    clients.push(res);

    req.on('close', () => {
        clients = clients.filter(client => client !== res);
    });
});

(async () => {
    try {
        await subscriber.connect();
        console.log("Connected to Redis");

        await subscriber.subscribe('SPOT_DATA', (message) => {
            const data = JSON.parse(message);
            const event = `data: ${JSON.stringify(data)}\n\n`;
            clients.forEach(client => client.write(event));
        });
    } catch (err) {
        console.error("Redis connection or subscription error:", err);
    }
})();

app.listen(4000, () => console.log('Server Running on port 4000'));
