#include "NodeDiscovery.h"
#include <algorithm>
#include <random>

NodeDiscovery::NodeDiscovery(const std::string& node_id, int port)
    : node_id_(node_id), address_("localhost"), port_(port) {
    
    NodeInfo self_info;
    self_info.node_id = node_id_;
    self_info.address = address_;
    self_info.port = port_;
    self_info.last_heartbeat = std::chrono::steady_clock::now();
    self_info.is_alive = true;
    
    known_nodes_[node_id_] = self_info;
}

void NodeDiscovery::broadcast_heartbeat() {
    std::lock_guard<std::mutex> lock(nodes_mutex_);
    
    auto now = std::chrono::steady_clock::now();
    known_nodes_[node_id_].last_heartbeat = now;
    
    std::cout << "[Discovery] " << node_id_ << " broadcasting heartbeat to " 
              << (known_nodes_.size() - 1) << " peers\n";
    
    simulate_heartbeat_exchange();
    cleanup_dead_nodes();
}

void NodeDiscovery::listen_for_heartbeats() {
    std::cout << "[Discovery] " << node_id_ << " listening for heartbeats\n";
    simulate_heartbeat_exchange();
}

void NodeDiscovery::simulate_heartbeat_exchange() {
    auto now = std::chrono::steady_clock::now();
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);
    
    for (auto& [node_id, node_info] : known_nodes_) {
        if (node_id == node_id_) continue;
        
        if (dis(gen) < 0.95) {
            node_info.last_heartbeat = now;
            node_info.is_alive = true;
        }
    }
}

void NodeDiscovery::cleanup_dead_nodes() {
    auto now = std::chrono::steady_clock::now();
    auto timeout = std::chrono::milliseconds(NODE_TIMEOUT_MS);
    
    for (auto& [node_id, node_info] : known_nodes_) {
        if (node_id != node_id_ && 
            (now - node_info.last_heartbeat) > timeout) {
            node_info.is_alive = false;
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