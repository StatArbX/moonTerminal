#include "../lib/websocket_client.h"
#include "../lib/binance_spot_handler.h"
#include "../lib/logger.h"

int main() {
    Logger logger;
    BinanceSpotHandler handler(logger);
    WebSocketClient client("stream.binance.com", "9443", "/ws/btcusdt@trade", handler, logger);
    client.connect();
    return 0; 
}