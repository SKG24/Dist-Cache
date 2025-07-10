#pragma once
#include <iostream>
#include <thread>
#include <atomic>
#include "storage/LRUCache.h"
#include "storage/WAL.h"
#include "cluster/HashRing.h"
#include "patterns/CircuitBreaker.h"
#include "monitoring/MetricsCollector.h"

class TCPServer {
public:
    TCPServer(int port, LRUCache& cache, WAL& wal, HashRing& hash_ring,
              CircuitBreaker& circuit_breaker, MetricsCollector& metrics);
    
    void start();
    int get_connection_count() const { return connection_count_; }
    
private:
    int port_;
    LRUCache& cache_;
    WAL& wal_;
    HashRing& hash_ring_;
    CircuitBreaker& circuit_breaker_;
    MetricsCollector& metrics_;
    std::atomic<int> connection_count_{0};
    
    void handle_client_simulation();
    bool process_command(const std::string& cmd, const std::string& key, 
                        const std::string& value, std::string& response);
};
