#ifndef WEBSOCKET_CLIENT_H
#define WEBSOCKET_CLIENT_H

#include "data_handler.h"
#include "logger.h"
#include <boost/beast.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>


namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace net = boost::asio;
namespace ssl = boost::asio::ssl;
using tcp = net::ip::tcp;


class WebSocketClient {
    public:
        WebSocketClient(const std::string& host, const std::string& port, const std::string& target,
                        DataHandler& handler, Logger& logger);
        void connect();
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
    
        void read_message();
    };
    
#endif 