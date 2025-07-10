#include "MetricsCollector.h"
#include <sstream>
#include <iomanip>

void MetricsCollector::record_latency(double ms) {
    total_latency_ += ms;
    request_count_++;
}

void MetricsCollector::increment_counter(const std::string& name) {
    counters_[name]++;
}

void MetricsCollector::record_active_connections(int count) {
    active_connections_ = count;
}

std::string MetricsCollector::generate_json() {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2);
    ss << "{";
    ss << "\"avg_latency\":" << (request_count_ > 0 ? total_latency_ / request_count_ : 0.0);
    ss << ",\"requests\":" << request_count_;
    ss << ",\"connections\":" << active_connections_;
    
    for (const auto& [name, count] : counters_) {
        ss << ",\"" << name << "\":" << count.load();
    }
    
    ss << "}";
    return ss.str();
}