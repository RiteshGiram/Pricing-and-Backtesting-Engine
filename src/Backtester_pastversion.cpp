#include "../include/Backtester.h"
#include "../include/Logger.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip>
#include <ctime>
#include <cmath>

// Standard Time Utility
double yearsBetween(const std::string& start, const std::string& end) {
    std::tm tm_start = {}, tm_end = {};
    std::istringstream ss1(start), ss2(end);
    ss1 >> std::get_time(&tm_start, "%Y-%m-%d");
    ss2 >> std::get_time(&tm_end, "%Y-%m-%d");
    if (ss1.fail() || ss2.fail()) return 0.0;
    return std::difftime(std::mktime(&tm_end), std::mktime(&tm_start)) / (365.25 * 24 * 3600);
}

Backtester::Backtester(std::shared_ptr<PricingEngine> engine) : engine_(engine) {}

void Backtester::run(const std::string& csvPath, double strike, const std::string& expiryDate, bool isCall, bool isBloomberg) {
    std::ifstream file(csvPath);
    if (!file.is_open()) {
        Logger::instance().log(LogLevel::ERROR, "Could not open data: " + csvPath);
        return;
    }
    
    std::string logPath = "logs/result_" + engine_->getName() + (isBloomberg ? "_BBG.csv" : ".csv");
    std::ofstream outfile(logPath);
    // Track Hedging Error instead of raw PnL
    outfile << "Date,Spot,T,OptionPrice,Delta,StockPos,Cash,PortfolioValue,HedgingError\n";
    
    std::string line;
    std::getline(file, line); // Header
    
    double cash = 0.0; 
    double stockPos = 0.0;
    bool isFirstTrade = true;

    Logger::instance().log(LogLevel::INFO, "Running Self-Financing Hedge: " + engine_->getName());

    while(std::getline(file, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string date, val;
        double spot, rate, vol;

        try {
            std::getline(ss, date, ','); 
            if (isBloomberg) {
                std::getline(ss, val, ','); spot = std::stod(val);
                std::getline(ss, val, ','); rate = std::stod(val)/100.0;
                std::getline(ss, val, ','); vol = std::stod(val)/100.0;
            } else {
                std::getline(ss, val, ','); spot = std::stod(val);
                std::getline(ss, val, ','); rate = std::stod(val);
                std::getline(ss, val, ','); vol = std::stod(val);
            }

            double T = yearsBetween(date, expiryDate);
            if (T < 0) break;

            double price = engine_->calculatePrice(spot, strike, rate, vol, T, isCall);
            double delta = engine_->calculateDelta(spot, strike, rate, vol, T, isCall);

            // --- SELF-FINANCING LOGIC ---
            if (isFirstTrade) {
                // Initializing: Receieve premium, Buy initial Delta
                cash = price; 
                isFirstTrade = false;
            }

            // Rebalance the hedge
            double tradeSize = delta - stockPos;
            cash -= (tradeSize * spot);
            stockPos = delta;

            // Portfolio Value = Cash + Value of Stocks held
            double portfolioValue = cash + (stockPos * spot);
            
            // Hedging Error = Current Portfolio Value - Current Option Fair Value
            // In a perfect world, this is 0. 
            double hedgingError = portfolioValue - price;

            outfile << date << "," << spot << "," << T << "," << price << "," 
                    << delta << "," << stockPos << "," << cash << "," 
                    << portfolioValue << "," << hedgingError << "\n";

        } catch (...) { continue; }
    }
    Logger::instance().log(LogLevel::INFO, "Backtest Complete: " + logPath);
}