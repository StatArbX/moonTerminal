#include "../lib/websocket_client.h"      // Relative path from src to lib
#include "../lib/data_handler.h"          // Relative path from src to lib
#include "../lib/logger.h"                // Relative path from src to lib
#include <thread>
#include <chrono>

WebSocketClient::WebSocketClient(const std::string& host, const std::string& port, const std::string& target,
                                 DataHandler& handler, Logger& logger)
    : host(host), port(port), target(target), handler(handler), logger(logger),
      resolver(ioc), ctx(ssl::context::tlsv12_client), ws(ioc, ctx) {
    ctx.set_default_verify_paths(); 
}

void WebSocketClient::connect() {
    while (true) {
        try {
            
            auto const results = resolver.resolve(host, port);

            
            ws.next_layer().set_verify_mode(ssl::verify_peer);
            ws.next_layer().set_verify_callback(ssl::host_name_verification(host));

            
            boost::asio::connect(beast::get_lowest_layer(ws), results);
            
            ws.next_layer().handshake(ssl::stream_base::client);

            
            ws.handshake(host, target);
            logger.log_info("WebSocket connection established");

            
            read_message();

            
            ioc.run();
            ioc.restart(); 
        } catch (const std::exception& e) {
            logger.log_error("Connection failed: " + std::string(e.what()));
            std::this_thread::sleep_for(std::chrono::seconds(5)); 
        }
    }
}

void WebSocketClient::read_message() {
    ws.async_read(buffer, [this](beast::error_code ec, std::size_t bytes_transferred) {
        if (!ec) {
            std::string message = beast::buffers_to_string(buffer.data());
            buffer.consume(bytes_transferred);
            handler.on_message(message);
            read_message(); 
        } else {
            logger.log_error("Read error: " + ec.message());
            ws.async_close(websocket::close_code::normal, [this](beast::error_code close_ec) {
                if (close_ec) {
                    logger.log_error("Close error: " + close_ec.message());
                }
                logger.log_info("WebSocket connection closed");
                ioc.stop(); 
            });
        }
    });
}