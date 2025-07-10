#include "HttpDashboard.h"
#include <thread>
#include <chrono>

HttpDashboard::HttpDashboard(MetricsCollector& metrics, HashRing& hash_ring,
                             CircuitBreaker& circuit_breaker)
    : metrics_(metrics), hash_ring_(hash_ring), circuit_breaker_(circuit_breaker) {}

void HttpDashboard::start(int port) {
    std::cout << "[Dashboard] HTTP dashboard running on port " << port << "\n";
    
    // Simulate dashboard updates
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(10));
        generate_status_report();
    }
}

void HttpDashboard::generate_status_report() {
    std::cout << "\n=== DistCache Status Report ===\n";
    std::cout << "Circuit Breaker: " << circuit_breaker_.get_state_string() 
              << " (failures: " << circuit_breaker_.get_failure_count() << ")\n";
    
    auto nodes = hash_ring_.get_all_nodes();
    std::cout << "Hash Ring: " << nodes.size() << " nodes [";
    for (size_t i = 0; i < nodes.size(); ++i) {
        std::cout << nodes[i];
        if (i < nodes.size() - 1) std::cout << ", ";
    }
    std::cout << "]\n";
    
    std::cout << "Metrics: " << metrics_.generate_json() << "\n";
    std::cout << "===============================\n\n";
}