#pragma once
#include "PricingEngine.h"
#include <memory>
#include <string>

class Backtester {
public:
    explicit Backtester(std::shared_ptr<PricingEngine> engine);
    // Runs the simulation. 'expiryDate' is strictly YYYY-MM-DD
    void run(const std::string& csvPath, double strike, const std::string& expiryDate, bool isCall, bool isBloomberg);
private:
    std::shared_ptr<PricingEngine> engine_;
};