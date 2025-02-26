#include "../lib/binance_spot_handler.h"  
#include "../lib/data_handler.h"          
#include <nlohmann/json.hpp>              
#include <iostream>


std::string country = "IND";


BinanceSpotHandler::BinanceSpotHandler(Logger& logger, const std::string& redis_host) : logger(logger), 
                                redis(std::make_shared<sw::redis::Redis>(redis_host)) {}

void BinanceSpotHandler::on_message(const std::string& message){
    try {
        if(redis){
            redis->publish("SPOT_DATA", message);
        } else {
            logger.log_error("Redis Connection Unavailable for broadcast");
        }

        auto data = nlohmann::json::parse(message);
        std::string symbol = data.at("s").get<std::string>();
        std::string price = data.at("p").get<std::string>();
        std::string quantity = data.at("q").get<std::string>();
        long int time = data.at("T").get<long int>();
        bool marketMaker = data.at("m").get<bool>();

        std::string maker;

        if(marketMaker){
            maker = "true";
        } else {
            maker = "false";
        }

        auto timestampToString = [time]() -> std::string {
            auto seconds = time/1000;
            auto milliseconds = time%1000;
            std::time_t time_t = seconds;
            std::tm* localtime = std::localtime(&time_t);
            char buffer[32];
            std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localtime);
            
            std::ostringstream oss;
            // oss << buffer <<"."<<std::setfill("0") << std::setw(3) << milliseconds;
            std::string result = std::format("{}.{:03d}", buffer, milliseconds);
            return result;
        };
        std::string timestampstr = timestampToString();

        nlohmann::json broadcast_data;
        broadcast_data["symbol"] = symbol;
        broadcast_data["price"] = price;
        broadcast_data["qty"] = quantity;
        broadcast_data["marketMaker"] = maker;
        broadcast_data["timestamp"] = timestampstr;

        
        
    } catch (const nlohmann::json::parse_error& e) {
        logger.log_error("Failed to parse Json: " + std::string(e.what()) + " - Raw Message" + message);
    } catch (const nlohmann::json::out_of_range& e) {
        logger.log_error("Missing Key: " + std::string(e.what()) + " - Raw Message" + message);
    } catch (const std::exception& e) {
        logger.log_error("Unexpected Error Processing Message: " + std::string(e.what()) + " - Raw Message" + message);
    }
}

