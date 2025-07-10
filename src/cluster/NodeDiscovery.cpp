#include "NodeDiscovery.h"
#include <algorithm>
#include <random>

NodeDiscovery::NodeDiscovery(const std::string& node_id, int port)
    : node_id_(node_id), address_("localhost"), port_(port) {
    
    // Add self to known nodes
    NodeInfo self_info;
    self_info.node_id = node_id_;
    self_info.address = address_;
    self_info.port = port_;
    self_info.last_heartbeat = std::chrono::steady_clock::now();
    self_info.is_alive = true;
    
    known_nodes_[node_id_] = self_info;
}

void NodeDiscovery::start_discovery() {
    running_ = true;
    
    heartbeat_thread_ = std::thread([this]() {
        while (running_) {
            broadcast_heartbeat();
            std::this_thread::sleep_for(std::chrono::milliseconds(HEARTBEAT_INTERVAL_MS));
        }
    });
    
    listener_thread_ = std::thread([this]() {
        while (running_) {
            listen_for_heartbeats();
            std::this_thread::sleep_for(std::chrono::milliseconds(HEARTBEAT_INTERVAL_MS / 2));
        }
    });
}

void NodeDiscovery::stop_discovery() {
    running_ = false;
    
    if (heartbeat_thread_.joinable()) {
        heartbeat_thread_.join();
    }
    
    if (listener_thread_.joinable()) {
        listener_thread_.join();
    }
}

void NodeDiscovery::broadcast_heartbeat() {
    std::lock_guard<std::mutex> lock(nodes_mutex_);
    
    auto now = std::chrono::steady_clock::now();
    known_nodes_[node_id_].last_heartbeat = now;
    
    std::cout << "[Discovery] " << node_id_ << " broadcasting heartbeat to " 
              << (known_nodes_.size() - 1) << " peers\n";
    
    // Simulate heartbeat exchange with other nodes
    simulate_heartbeat_exchange();
    
    // Cleanup dead nodes
    cleanup_dead_nodes();
}

void NodeDiscovery::listen_for_heartbeats() {
    std::cout << "[Discovery] " << node_id_ << " listening for heartbeats\n";
    
    // Simulate receiving heartbeats from other nodes
    simulate_heartbeat_exchange();
}

void NodeDiscovery::simulate_heartbeat_exchange() {
    // Simulate network heartbeat exchange
    auto now = std::chrono::steady_clock::now();
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);
    
    for (auto& [node_id, node_info] : known_nodes_) {
        if (node_id == node_id_) continue; // Skip self
        
        // Simulate network reliability (95% success rate)
        if (dis(gen) < 0.95) {
            node_info.last_heartbeat = now;
            node_info.is_alive = true;
        }
    }
}

void NodeDiscovery::cleanup_dead_nodes() {
    auto now = std::chrono::steady_clock::now();
    auto timeout = std::chrono::milliseconds(NODE_TIMEOUT_MS);
    
    auto it = known_nodes_.begin();
    while (it != known_nodes_.end()) {
        if (it->first != node_id_ && // Don't remove self
            (now - it->second.last_heartbeat) > timeout) {
            
            std::cout << "[Discovery] Node " << it->first << " marked as dead\n";
            it->second.is_alive = false;
            ++it;
        } else {
            ++it;
        }
    }
}

std::vector<NodeDiscovery::NodeInfo> NodeDiscovery::get_alive_nodes() const {
    std::lock_guard<std::mutex> lock(nodes_mutex_);
    
    std::vector<NodeInfo> alive_nodes;
    for (const auto& [node_id, node_info] : known_nodes_) {
        if (node_info.is_alive) {
            alive_nodes.push_back(node_info);
        }
    }
    
    return alive_nodes;
}

bool NodeDiscovery::is_node_alive(const std::string& node_id) const {
    std::lock_guard<std::mutex> lock(nodes_mutex_);
    
    auto it = known_nodes_.find(node_id);
    return it != known_nodes_.end() && it->second.is_alive;
}

void NodeDiscovery::add_seed_node(const std::string& node_id, const std::string& address, int port) {
    std::lock_guard<std::mutex> lock(nodes_mutex_);
    
    NodeInfo node_info;
    node_info.node_id = node_id;
    node_info.address = address;
    node_info.port = port;
    node_info.last_heartbeat = std::chrono::steady_clock::now();
    node_info.is_alive = true;
    
    known_nodes_[node_id] = node_info;
    
    std::cout << "[Discovery] Added seed node: " << node_id 
              << " at " << address << ":" << port << "\n";
}