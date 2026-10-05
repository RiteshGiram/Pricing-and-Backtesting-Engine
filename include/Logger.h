#pragma once
#include <string>
#include <fstream>
#include <mutex>
#include <iostream>

enum class LogLevel { INFO, WARNING, ERROR };

class Logger {
public:
    static Logger& instance();
    void init(const std::string& filename);
    void log(LogLevel level, const std::string& msg);
private:
    Logger() {}
    std::ofstream logFile_;
    std::mutex mtx_;
};