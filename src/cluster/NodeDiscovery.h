#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>

class NodeDiscovery {
public:
    struct NodeInfo {
        std::string node_id;
        std::string address;
        int port;
        std::chrono::steady_clock::time_point last_heartbeat;
        bool is_alive;
    };
    
    explicit NodeDiscovery(const std::string& node_id = "localhost", int port = 6379);
    
    void broadcast_heartbeat();
    void listen_for_heartbeats();
    void start_discovery() { 
        std::cout << "[Discovery] Starting discovery for " << node_id_ << "\n";
    }
    void stop_discovery() { 
        std::cout << "[Discovery] Stopping discovery for " << node_id_ << "\n";
    }
    
    std::vector<NodeInfo> get_alive_nodes() const;
    bool is_node_alive(const std::string& node_id) const;
    void add_seed_node(const std::string& node_id, const std::string& address, int port);
    
private:
    std::string node_id_;
    std::string address_;
    int port_;
    
    std::map<std::string, NodeInfo> known_nodes_;
    mutable std::mutex nodes_mutex_;
    
    void cleanup_dead_nodes();
    void simulate_heartbeat_exchange();
    static constexpr int HEARTBEAT_INTERVAL_MS = 5000;
    static constexpr int NODE_TIMEOUT_MS = 15000;
};