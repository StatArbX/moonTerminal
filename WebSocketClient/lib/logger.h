#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <fstream>
#include <iostream>
#include <ctime>


class Logger {
    public:
        Logger(const std::string& log_file = "binance_listener.log");
        ~Logger();
        void log_error(const std::string& message);
        void log_info(const std::string& message);
    private:
        std::ofstream log_stream;
        void log(const std::string& level, const std::string& message);
};


#endif