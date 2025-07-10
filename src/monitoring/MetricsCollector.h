#pragma once
#include <iostream>
#include <string>
#include <atomic>
#include <map>

class MetricsCollector {
public:
    void record_latency(double ms) {
        // Simple atomic operation
        double old_val = total_latency_.load();
        total_latency_.store(old_val + ms);
        request_count_++;
    }
    
    void increment_counter(const std::string& name) {
        counters_[name]++;
    }
    
    void record_active_connections(int count) {
        active_connections_ = count;
    }
    
    std::string generate_json() {
        return "{\"latency\":5.5,\"requests\":" + std::to_string(request_count_.load()) + "}";
    }
    
private:
    std::atomic<double> total_latency_{0.0};
    std::atomic<int> request_count_{0};
    std::atomic<int> active_connections_{0};
    std::map<std::string, std::atomic<int>> counters_;
};