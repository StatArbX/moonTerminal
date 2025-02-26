#ifndef BINANCE_SPOT_HANDLER_H
#define BINANCE_SPOT_HANDLER_H

#include "data_handler.h"
#include "logger.h"
#include <string>

#include <nlohmann/json.hpp>
#include <sw/redis++/redis++.h>
#include <iostream>
#include <format>
#include <nlohmann/json.hpp>

using nlohmann::json;


class BinanceSpotHandler : public DataHandler {
    public:
        BinanceSpotHandler(Logger& logger, 
                    const std::string& redis_host = "redis://127.0.0.1:6379");
        void on_message(const std::string& message) override;
    private:
        Logger& logger;
        std::shared_ptr<sw::redis::Redis> redis;
};


#endif