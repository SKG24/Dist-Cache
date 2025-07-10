#include <iostream>
#include <thread>
#include <chrono>
#include <signal.h>
#include "storage/LRUCache.h"
#include "storage/WAL.h"
#include "storage/MMapPersistence.h"
#include "network/TCPServer.h"
#include "network/RESPParser.h"
#include "cluster/HashRing.h"
#include "cluster/NodeDiscovery.h"
#include "patterns/CircuitBreaker.h"
#include "monitoring/MetricsCollector.h"
#include "monitoring/HttpDashboard.h"

std::atomic<bool> shutdown_requested(false);

void signal_handler(int signal) {
    std::cout << "\n[DistCache] Shutdown signal received...\n";
    shutdown_requested = true;
}

int main() {
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    std::cout << "=== DistCache Enhanced v2.0 ===" << std::endl;
    std::cout << "[DistCache] Initializing distributed cache components...\n";
    
    // Core storage components
    LRUCache cache(10000);
    WAL wal("wal.log");
    MMapPersistence persistence("snapshot.dat");
    RESPParser parser;
    MetricsCollector metrics;
    
    // NEW: Distributed systems components
    HashRing hash_ring;
    CircuitBreaker circuit_breaker(5, 30000); // 5 failures, 30s timeout
    
    // Add some nodes to demonstrate consistent hashing
    hash_ring.add_node("node-1");
    hash_ring.add_node("node-2");  
    hash_ring.add_node("node-3");
    std::cout << "[HashRing] Initialized with 3 nodes\n";
    
    // Recovery from persistence
    std::cout << "[DistCache] Loading persisted data...\n";
    auto persisted_data = persistence.load();
    for (const auto& [key, val] : persisted_data) {
        cache.set(key, val);
    }
    
    // WAL replay
    std::cout << "[DistCache] Replaying WAL entries...\n";
    for (auto [op, key, val] : wal.replay()) {
        if (op == "SET") cache.set(key, val);
        else if (op == "DEL") cache.del(key);
    }
    
    // Network components
    std::cout << "[DistCache] Starting TCP server on port 6379...\n";
    TCPServer server(6379, cache, wal, hash_ring, circuit_breaker, metrics);
    std::thread server_thread([&]() { server.start(); });
    
    // Node discovery
    std::cout << "[DistCache] Launching node discovery...\n";
    NodeDiscovery discovery;
    std::thread discovery_thread([&]() {
        discovery.broadcast_heartbeat();
        discovery.listen_for_heartbeats();
    });
    
    // HTTP Dashboard
    std::cout << "[DistCache] Starting HTTP dashboard on port 8080...\n";
    HttpDashboard dashboard(metrics, hash_ring, circuit_breaker);
    std::thread dashboard_thread([&]() { dashboard.start(8080); });
    
    // Background cleanup
    std::thread cleanup_thread([&]() {
        while (!shutdown_requested) {
            std::this_thread::sleep_for(std::chrono::seconds(5));
            cache.cleanup_expired();
            metrics.record_active_connections(server.get_connection_count());
        }
    });
    
    std::cout << "[DistCache] System ready! Features enabled:\n";
    std::cout << "  ✓ Consistent Hashing for data distribution\n";
    std::cout << "  ✓ Circuit Breaker for failure protection\n"; 
    std::cout << "  ✓ LRU Cache with TTL support\n";
    std::cout << "  ✓ Write-Ahead Logging for durability\n";
    std::cout << "  ✓ Memory-mapped persistence\n";
    std::cout << "  ✓ Real-time metrics dashboard\n";
    std::cout << "\nPress Ctrl+C to shutdown gracefully.\n";
    
    // Wait for shutdown
    while (!shutdown_requested) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    std::cout << "[DistCache] Graceful shutdown initiated...\n";
    shutdown_requested = true;
    
    // Cleanup
    if (server_thread.joinable()) server_thread.join();
    if (discovery_thread.joinable()) discovery_thread.join();
    if (dashboard_thread.joinable()) dashboard_thread.join();
    if (cleanup_thread.joinable()) cleanup_thread.join();
    
    std::cout << "[DistCache] Shutdown complete.\n";
    return 0;
}
