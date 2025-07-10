#include "TCPServer.h"
#include <chrono>
#include <random>

TCPServer::TCPServer(int port, LRUCache& cache, WAL& wal, HashRing& hash_ring,
                     CircuitBreaker& circuit_breaker, MetricsCollector& metrics)
    : port_(port), cache_(cache), wal_(wal), hash_ring_(hash_ring),
      circuit_breaker_(circuit_breaker), metrics_(metrics) {}

void TCPServer::start() {
    std::cout << "[TCPServer] Listening on port " << port_ << "\n";
    
    // Simulate multiple client connections
    std::vector<std::thread> client_threads;
    for (int i = 0; i < 3; ++i) {
        client_threads.emplace_back([this]() { handle_client_simulation(); });
    }
    
    for (auto& t : client_threads) {
        if (t.joinable()) t.join();
    }
}

void TCPServer::handle_client_simulation() {
    connection_count_++;
    std::cout << "[TCPServer] Client connected (total: " << connection_count_ << ")\n";
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> cmd_dist(0, 2);
    std::uniform_int_distribution<> key_dist(1, 1000);
    
    // Simulate client requests
    for (int i = 0; i < 10; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
        if (!circuit_breaker_.allow_request()) {
            std::cout << "[TCPServer] Request blocked by circuit breaker\n";
            metrics_.increment_counter("requests_blocked");
            continue;
        }
        
        std::string cmd, key = "key" + std::to_string(key_dist(gen)), value, response;
        
        switch (cmd_dist(gen)) {
            case 0: cmd = "SET"; value = "value" + std::to_string(i); break;
            case 1: cmd = "GET"; break;
            case 2: cmd = "DEL"; break;
        }
        
        auto start = std::chrono::high_resolution_clock::now();
        bool success = process_command(cmd, key, value, response);
        auto end = std::chrono::high_resolution_clock::now();
        
        double latency = std::chrono::duration<double, std::milli>(end - start).count();
        metrics_.record_latency(latency);
        
        if (success) {
            circuit_breaker_.record_success();
            metrics_.increment_counter("requests_success");
        } else {
            circuit_breaker_.record_failure();
            metrics_.increment_counter("requests_failed");
        }
        
        std::cout << "[TCPServer] " << cmd << " " << key 
                  << " -> " << response << " (" << latency << "ms)\n";
    }
    
    connection_count_--;
    std::cout << "[TCPServer] Client disconnected (total: " << connection_count_ << ")\n";
}

bool TCPServer::process_command(const std::string& cmd, const std::string& key,
                               const std::string& value, std::string& response) {
    // Check which node should handle this key
    std::string target_node = hash_ring_.get_node(key);
    std::cout << "[HashRing] Key '" << key << "' -> Node '" << target_node << "'\n";
    
    try {
        if (cmd == "SET") {
            cache_.set(key, value);
            wal_.append("SET", key, value);
            response = "OK";
            return true;
        } else if (cmd == "GET") {
            std::string val;
            if (cache_.get(key, val)) {
                response = val;
            } else {
                response = "(nil)";
            }
            return true;
        } else if (cmd == "DEL") {
            cache_.del(key);
            wal_.append("DEL", key);
            response = "OK";
            return true;
        }
        
        response = "ERR unknown command";
        return false;
    } catch (const std::exception& e) {
        response = "ERR " + std::string(e.what());
        return false;
    }
}