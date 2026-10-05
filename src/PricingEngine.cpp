#include "../include/PricingEngine.h"
#include <cmath>
#include <vector>
#include <algorithm>
#include <random>
#include <thread>
#include <future>

double norm_cdf(double value) {
    return 0.5 * std::erfc(-value * M_SQRT1_2);
}

// 1. Black-Scholes
double BlackScholesEngine::calculatePrice(double S, double K, double r, double v, double T, bool isCall) {
    if (T <= 0.001) return (isCall ? std::max(S-K,0.0) : std::max(K-S,0.0));
    double d1 = (std::log(S/K) + (r + 0.5*v*v)*T) / (v*std::sqrt(T));
    double d2 = d1 - v*std::sqrt(T);
    if (isCall) return S * norm_cdf(d1) - K * std::exp(-r*T) * norm_cdf(d2);
    else return K * std::exp(-r*T) * norm_cdf(-d2) - S * norm_cdf(-d1);
}

double BlackScholesEngine::calculateDelta(double S, double K, double r, double v, double T, bool isCall) {
    if (T <= 0.001) return 0.0;
    double d1 = (std::log(S/K) + (r + 0.5*v*v)*T) / (v*std::sqrt(T));
    return isCall ? norm_cdf(d1) : norm_cdf(d1) - 1.0;
}

// 2. Binomial Tree
double BinomialTreeEngine::calculatePrice(double S, double K, double r, double v, double T, bool isCall) {
    if (T <= 0.001) return (isCall ? std::max(S-K,0.0) : std::max(K-S,0.0));
    double dt = T / steps_;
    double u = std::exp(v * std::sqrt(dt));
    double d = 1.0 / u;
    double p = (std::exp(r * dt) - d) / (u - d);
    double discount = std::exp(-r * dt);
    std::vector<double> values(steps_ + 1);
    for (int i = 0; i <= steps_; ++i) {
        double ST = S * std::pow(u, steps_ - i) * std::pow(d, i);
        values[i] = isCall ? std::max(0.0, ST - K) : std::max(0.0, K - ST);
    }
    for (int j = steps_ - 1; j >= 0; --j) {
        for (int i = 0; i <= j; ++i) {
            values[i] = discount * (p * values[i] + (1 - p) * values[i + 1]);
        }
    }
    return values[0];
}

double BinomialTreeEngine::calculateDelta(double S, double K, double r, double v, double T, bool isCall) {
    if (T <= 0.001) return 0.0;
    double dt = T / steps_;
    double u = std::exp(v * std::sqrt(dt));
    double d = 1.0 / u;
    double P_up = calculatePrice(S * u, K, r, v, T - dt, isCall);
    double P_down = calculatePrice(S * d, K, r, v, T - dt, isCall);
    return (P_up - P_down) / (S * u - S * d);
}

// 3. Monte Carlo (Standard Threads)
double MonteCarloEngine::calculatePrice(double S, double K, double r, double v, double T, bool isCall) {
    if (T <= 0.001) return (isCall ? std::max(S-K,0.0) : std::max(K-S,0.0));

    unsigned int num_threads = std::thread::hardware_concurrency();
    if (num_threads == 0) num_threads = 2;
    int sims_per_thread = num_simulations_ / num_threads;
    
    std::vector<std::future<double>> futures;

    for (unsigned int t = 0; t < num_threads; ++t) {
        futures.push_back(std::async(std::launch::async, [=]() {
            std::hash<std::thread::id> hasher;
            unsigned int seed = 42 + hasher(std::this_thread::get_id());
            std::mt19937 rng(seed);
            std::normal_distribution<double> gaussian(0.0, 1.0);
            
            double dt = T / num_steps_;
            double drift = (r - 0.5 * v * v) * dt;
            double vol_sqrt_dt = v * std::sqrt(dt);
            double local_sum = 0.0;

            for (int i = 0; i < sims_per_thread; ++i) {
                double current_S = S;
                for (int j = 0; j < num_steps_; ++j) {
                    current_S *= std::exp(drift + vol_sqrt_dt * gaussian(rng));
                }
                local_sum += (isCall ? std::max(current_S - K, 0.0) : std::max(K - current_S, 0.0));
            }
            return local_sum;
        }));
    }

    double total_payoff = 0.0;
    for (auto& f : futures) total_payoff += f.get();
    
    return (total_payoff / (sims_per_thread * num_threads)) * std::exp(-r * T);
}

double MonteCarloEngine::calculateDelta(double S, double K, double r, double v, double T, bool isCall) {
    double h = S * 0.01;
    double P0 = calculatePrice(S, K, r, v, T, isCall);
    double P1 = calculatePrice(S + h, K, r, v, T, isCall);
    return (P1 - P0) / h;
}