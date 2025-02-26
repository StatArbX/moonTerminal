#include <boost/beast.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <nlohmann/json.hpp>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>
#include <iostream>
#include <ctime>



namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace net = boost::asio;
namespace ssl = boost::asio::ssl;
using tcp = net::ip::tcp;

// Logger class for logging events and errors to a file
class Logger {
public:
    Logger(const std::string& log_file = "binance_listener.log") {
        log_stream.open(log_file, std::ios::app);
        if (!log_stream.is_open()) {
            std::cerr << "Failed to open log file: " << log_file << std::endl;
        }
    }

    ~Logger() {
        if (log_stream.is_open()) {
            log_stream.close();
        }
    }

    void log_error(const std::string& message) {
        log("ERROR", message);
    }

    void log_info(const std::string& message) {
        log("INFO", message);
    }

private:
    std::ofstream log_stream;

    void log(const std::string& level, const std::string& message) {
        if (log_stream.is_open()) {
            time_t now = time(nullptr);
            char* dt = ctime(&now);
            std::string timestamp(dt);
            timestamp.erase(timestamp.find_last_not_of("\n\r") + 1); // Trim newline from ctime
            log_stream << timestamp << " - " << level << " - " << message << std::endl;
            log_stream.flush(); // Ensure immediate write to file
        }
    }
};

// Abstract base class for handling WebSocket messages
class DataHandler {
public:
    virtual ~DataHandler() = default;
    virtual void on_message(const std::string& message) = 0;
};

// Handler for Binance spot trade data
class BinanceSpotHandler : public DataHandler {
public:
    BinanceSpotHandler(Logger& logger) : logger(logger) {}

    void on_message(const std::string& message) override {
        try {
            auto data = nlohmann::json::parse(message);
            std::string symbol = data.at("s").get<std::string>();
            std::string price = data.at("p").get<std::string>();
            std::string quantity = data.at("q").get<std::string>();
            std::cout << "Trade: " << symbol << " at " << price << ", quantity " << quantity << std::endl;
        } catch (const nlohmann::json::parse_error& e) {
            logger.log_error("Failed to parse JSON: " + std::string(e.what()) + " - Raw message: " + message);
        } catch (const nlohmann::json::out_of_range& e) {
            logger.log_error("Missing expected key: " + std::string(e.what()) + " - Message: " + message);
        } catch (const std::exception& e) {
            logger.log_error("Unexpected error processing message: " + std::string(e.what()) + " - Message: " + message);
        }
    }

private:
    Logger& logger;
};

// WebSocket client with reconnection logic
class WebSocketClient {
public:
    WebSocketClient(const std::string& host, const std::string& port, const std::string& target,
                    DataHandler& handler, Logger& logger)
        : host(host), port(port), target(target), handler(handler), logger(logger),
          resolver(ioc), ctx(ssl::context::tlsv12_client), ws(ioc, ctx) {
        ctx.set_default_verify_paths(); // Use system's default certificate paths
    }

    void connect() {
        while (true) {
            try {
                // Resolve the host
                auto const results = resolver.resolve(host, port);

                // Configure SSL verification
                ws.next_layer().set_verify_mode(ssl::verify_peer);
                ws.next_layer().set_verify_callback(ssl::host_name_verification(host));

                // Connect to the server
                boost::asio::connect(beast::get_lowest_layer(ws), results);
                // Perform SSL handshake
                ws.next_layer().handshake(ssl::stream_base::client);

                // Perform WebSocket handshake
                ws.handshake(host, target);
                logger.log_info("WebSocket connection established");

                // Start reading messages asynchronously
                read_message();

                // Run the I/O context (blocks until all operations complete)
                ioc.run();
                ioc.restart(); // Reset the I/O context for the next connection attempt
            } catch (const std::exception& e) {
                logger.log_error("Connection failed: " + std::string(e.what()));
                std::this_thread::sleep_for(std::chrono::seconds(5)); // Reconnect delay
            }
        }
    }

private:
    std::string host;
    std::string port;
    std::string target;
    DataHandler& handler;
    Logger& logger;
    net::io_context ioc;
    ssl::context ctx;
    tcp::resolver resolver;
    websocket::stream<beast::ssl_stream<tcp::socket>> ws;
    beast::flat_buffer buffer;

    void read_message() {
        ws.async_read(buffer, [this](beast::error_code ec, std::size_t bytes_transferred) {
            if (!ec) {
                std::string message = beast::buffers_to_string(buffer.data());
                buffer.consume(bytes_transferred);
                handler.on_message(message);
                read_message(); // Continue reading the next message
            } else {
                logger.log_error("Read error: " + ec.message());
                ws.async_close(websocket::close_code::normal, [this](beast::error_code close_ec) {
                    if (close_ec) {
                        logger.log_error("Close error: " + close_ec.message());
                    }
                    logger.log_info("WebSocket connection closed");
                    ioc.stop(); // Stop the I/O context to exit run()
                });
            }
        });
    }
};

// Main function to set up and run the WebSocket client
int main() {
    Logger logger;
    BinanceSpotHandler handler(logger);
    WebSocketClient client("stream.binance.com", "9443", "/ws/btcusdt@trade", handler, logger);
    client.connect();
    return 0; // Note: This line is never reached due to the infinite loop in connect()
}