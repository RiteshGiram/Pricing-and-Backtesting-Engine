#include "../include/Logger.h"

Logger& Logger::instance() {
    static Logger instance;
    return instance;
}

void Logger::init(const std::string& filename) {
    std::lock_guard<std::mutex> lock(mtx_);
    logFile_.open(filename, std::ios::out | std::ios::app);
}

void Logger::log(LogLevel level, const std::string& msg) {
    std::lock_guard<std::mutex> lock(mtx_);
    if (logFile_.is_open()) logFile_ << msg << std::endl;
    // Optional: Print ERRORs to console so you see them immediately
    if (level == LogLevel::ERROR) std::cerr << "[ERROR] " << msg << std::endl;
}