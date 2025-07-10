#include "HashRing.h"

uint32_t HashRing::hash(const std::string& input) const {
    // Simple but effective hash function (FNV-1a variant)
    uint32_t hash = 2166136261u;
    for (char c : input) {
        hash ^= static_cast<uint32_t>(c);
        hash *= 16777619u;
    }
    return hash;
}

void HashRing::add_node(const std::string& node_id) {
    // Add virtual nodes for better load distribution
    for (int i = 0; i < VIRTUAL_NODES; ++i) {
        std::string virtual_node = node_id + ":" + std::to_string(i);
        uint32_t hash_val = hash(virtual_node);
        ring_[hash_val] = node_id;
    }
    std::cout << "[HashRing] Added node: " << node_id 
              << " (" << VIRTUAL_NODES << " virtual nodes)\n";
}

void HashRing::remove_node(const std::string& node_id) {
    auto it = ring_.begin();
    while (it != ring_.end()) {
        if (it->second == node_id) {
            it = ring_.erase(it);
        } else {
            ++it;
        }
    }
    std::cout << "[HashRing] Removed node: " << node_id << "\n";
}

std::string HashRing::get_node(const std::string& key) const {
    if (ring_.empty()) return "localhost"; // Fallback
    
    uint32_t key_hash = hash(key);
    
    // Find first node with hash >= key_hash
    auto it = ring_.lower_bound(key_hash);
    if (it == ring_.end()) {
        // Wrap around to first node
        it = ring_.begin();
    }
    
    return it->second;
}

std::vector<std::string> HashRing::get_all_nodes() const {
    std::vector<std::string> nodes;
    for (const auto& [hash_val, node] : ring_) {
        if (std::find(nodes.begin(), nodes.end(), node) == nodes.end()) {
            nodes.push_back(node);
        }
    }
    return nodes;
}

void HashRing::print_ring_status() const {
    std::cout << "[HashRing] Status: " << ring_.size() << " virtual nodes, "
              << get_all_nodes().size() << " physical nodes\n";
}