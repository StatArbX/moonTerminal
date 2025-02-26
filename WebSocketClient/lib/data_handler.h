#ifndef DATA_HANDLER_H
#define DATA_HANDLER_H

#include <string>

class DataHandler {
    public:
        virtual ~DataHandler() = default;
        virtual void on_message(const std::string& message) = 0;
};

#endif