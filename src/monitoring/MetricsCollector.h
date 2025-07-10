#pragma once
#include <iostream>
#include <string>
#include <atomic>
#include <map>

class MetricsCollector {
public:
    void record_latency(double ms);
    void increment_counter(const std::string& name);
    void record_active_connections(int count);
    std::string generate_json();
    
private:
    std::atomic<double> total_latency_{0.0};
    std::atomic<int> request_count_{0};
    std::atomic<int> active_connections_{0};
    std::map<std::string, std::atomic<int>> counters_;
};