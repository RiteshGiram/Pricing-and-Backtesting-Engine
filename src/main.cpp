#include "../include/PricingEngine.h"
#include "../include/Backtester.h"
#include "../include/Logger.h"
#include <vector>
#include <memory>

int main() {
    Logger::instance().init("logs/system.log");
    Logger::instance().log(LogLevel::INFO, "System Started");
    
    // 1. Define Models
    std::vector<std::shared_ptr<PricingEngine>> models;
    models.push_back(std::make_shared<BlackScholesEngine>());
    models.push_back(std::make_shared<BinomialTreeEngine>(100));
    models.push_back(std::make_shared<MonteCarloEngine>(5000, 252));

    // 2. Define Data Sources (Path, IsBloomberg?)
    std::vector<std::pair<std::string, bool>> datasets = {
        {"data/market_data_yfinance.csv", false},  // Source 1: Yahoo
        {"data/market_data_synthetic.csv", false},  // Source 2: Synthetic
        {"data/bloomberg_export.csv", true}         // Source 3: Bloomberg
    };

    // 3. Run All Combinations
    // We assume the option expires at the end of 2024
    std::string expiry = "2024-12-30";
    double strike = 100.0; 

    for (auto& data : datasets) {
        for (auto& model : models) {
            Backtester bt(model);
            bt.run(data.first, strike, expiry, true, data.second);
        }
    }
    
    return 0;
}