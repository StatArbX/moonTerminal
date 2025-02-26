import websocket
import json
import threading
import time
import logging
from abc import ABC, abstractmethod

class Logger:
    """Handles logging of application events and errors to a file."""
    def __init__(self, log_file='binance_listener.log'):
        logging.basicConfig(
            filename=log_file,
            level=logging.INFO,
            format='%(asctime)s - %(levelname)s - %(message)s'
        )

    def log_error(self, message):
        """Log an error message."""
        logging.error(message)

    def log_info(self, message):
        """Log an informational message."""
        logging.info(message)

class DataHandler(ABC):
    """Abstract base class for handling WebSocket messages."""
    @abstractmethod
    def on_message(self, message):
        """Process an incoming message."""
        pass

class BinanceSpotHandler(DataHandler):
    """Handles Binance spot trade data messages."""
    def on_message(self, message):
        try:
            data = json.loads(message)
            symbol = data['s']
            price = data['p']
            quantity = data['q']
            print(f"Trade: {symbol} at {price}, quantity {quantity}")
        except json.JSONDecodeError as e:
            logger.log_error(f"Failed to decode JSON message: {e} - Raw message: {message}")
        except KeyError as e:
            logger.log_error(f"Missing expected key '{e}' in message: {message}")
        except Exception as e:
            logger.log_error(f"Unexpected error processing message: {e} - Message: {message}")

class WebSocketClient:
    """Manages the WebSocket connection to Binance with reconnection logic."""
    def __init__(self, url, handler, logger):
        self.url = url
        self.handler = handler
        self.logger = logger
        self.ws = None
        self.reconnect_delay = 5  # Delay in seconds before reconnecting

    def on_open(self, ws):
        """Callback for when the WebSocket connection opens."""
        self.logger.log_info("WebSocket connection established")

    def on_message(self, ws, message):
        """Callback for incoming WebSocket messages."""
        self.handler.on_message(message)

    def on_error(self, ws, error):
        """Callback for WebSocket errors."""
        self.logger.log_error(f"WebSocket error occurred: {error}")

    def on_close(self, ws, close_status_code, close_msg):
        """Callback for when the WebSocket connection closes."""
        self.logger.log_info(f"WebSocket connection closed: Status {close_status_code} - {close_msg}")
        self.reconnect()

    def connect(self):
        """Start the WebSocket connection with reconnection logic."""
        while True:
            try:
                self.ws = websocket.WebSocketApp(
                    self.url,
                    on_open=self.on_open,
                    on_message=self.on_message,
                    on_error=self.on_error,
                    on_close=self.on_close
                )
                self.ws.run_forever()
            except Exception as e:
                self.logger.log_error(f"Connection failed: {e}")
                time.sleep(self.reconnect_delay)

    def reconnect(self):
        """Attempt to reconnect after a delay."""
        self.logger.log_info("Attempting to reconnect...")
        time.sleep(self.reconnect_delay)
        self.connect()

if __name__ == "__main__":
    # Initialize components
    logger = Logger()
    handler = BinanceSpotHandler()
    url = "wss://stream.binance.com:9443/ws/btcusdt@trade"
    client = WebSocketClient(url, handler, logger)

    # Start the WebSocket client
    client.connect()