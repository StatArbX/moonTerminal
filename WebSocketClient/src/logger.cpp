#include "../lib/logger.h"  
#include <iostream>
#include <ctime>




Logger::Logger(const std::string& log_file) {
    log_stream.open(log_file, std::ios::app);
    if(!log_stream.is_open()){
        std::cerr<<"Failed to open the log file"<<std::endl;
    }
}


Logger::~Logger(){
    if (log_stream.is_open()){
        log_stream.close();
    }
}

void Logger::log_error(const std::string& message) {
    log("ERROR", message);
}

void Logger::log_info(const std::string& message) {
    log("INFO", message);
}


void Logger::log(const std::string& level, const std::string& message){
    if (log_stream.is_open()){
        time_t now = time(nullptr);
        now += (5*3600) + (30*60);
        char* dt_str = ctime(&now);
        std::string timestamp(dt_str);
        timestamp.erase(timestamp.find_last_not_of("\n\r") + 1);
        log_stream << timestamp << " - " << " - " << message << std::endl;
        log_stream.flush();
    }
}