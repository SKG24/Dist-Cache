#pragma once
#include <map>
#include <string>
#include <vector>
#include <functional>
#include <algorithm>
#include <iostream>

class HashRing {
public:
    void add_node(const std::string& node_id);
    void remove_node(const std::string& node_id);
    std::string get_node(const std::string& key) const;
    std::vector<std::string> get_all_nodes() const;
    void print_ring_status() const;
    
private:
    std::map<uint32_t, std::string> ring_;
    static const int VIRTUAL_NODES = 3; // Simple virtual nodes for better distribution
    
    uint32_t hash(const std::string& input) const;
};