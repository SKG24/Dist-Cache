#pragma once
#include <iostream>
#include "MetricsCollector.h"
#include "cluster/HashRing.h"
#include "patterns/CircuitBreaker.h"

class HttpDashboard {
public:
    HttpDashboard(MetricsCollector& metrics, HashRing& hash_ring, 
                  CircuitBreaker& circuit_breaker);
    void start(int port);
    
private:
    MetricsCollector& metrics_;
    HashRing& hash_ring_;
    CircuitBreaker& circuit_breaker_;
    
    void generate_status_report();
};
