#pragma once
#include <string>

class PricingEngine {
public:
    virtual ~PricingEngine() = default;
    virtual double calculatePrice(double spot, double strike, double rate, double vol, double T, bool isCall) = 0;
    virtual double calculateDelta(double spot, double strike, double rate, double vol, double T, bool isCall) = 0;
    virtual std::string getName() const = 0;
};

class BlackScholesEngine : public PricingEngine {
public:
    double calculatePrice(double spot, double strike, double rate, double vol, double T, bool isCall) override;
    double calculateDelta(double spot, double strike, double rate, double vol, double T, bool isCall) override;
    std::string getName() const override { return "Black-Scholes"; }
};

class BinomialTreeEngine : public PricingEngine {
    int steps_;
public:
    explicit BinomialTreeEngine(int steps) : steps_(steps) {}
    double calculatePrice(double spot, double strike, double rate, double vol, double T, bool isCall) override;
    double calculateDelta(double spot, double strike, double rate, double vol, double T, bool isCall) override;
    std::string getName() const override { return "Binomial Tree"; }
};

class MonteCarloEngine : public PricingEngine {
    int num_simulations_;
    int num_steps_;
public:
    MonteCarloEngine(int sims, int steps) : num_simulations_(sims), num_steps_(steps) {}
    double calculatePrice(double spot, double strike, double rate, double vol, double T, bool isCall) override;
    double calculateDelta(double spot, double strike, double rate, double vol, double T, bool isCall) override;
    std::string getName() const override { return "Monte Carlo (Std Thread)"; }
};