#pragma once
#include <unordered_map>
#include <list>
#include <string>
#include <mutex>
#include <shared_mutex>
#include <chrono>
#include <atomic>

class LRUCache {
public:
    explicit LRUCache(size_t capacity);
    
    bool get(const std::string& key, std::string& value);
    void set(const std::string& key, const std::string& value, int ttl_seconds = -1);
    void del(const std::string& key);
    bool exists(const std::string& key);
    void cleanup_expired();
    
    // Enhanced monitoring methods
    size_t size() const;
    size_t capacity() const { return capacity_; }
    double hit_rate() const;
    void reset_stats();
    
    // Advanced operations
    std::vector<std::string> get_all_keys() const;
    void clear();
    bool set_if_not_exists(const std::string& key, const std::string& value, int ttl_seconds = -1);
    
private:
    struct Entry {
        std::string value;
        std::chrono::steady_clock::time_point expire_time;
        std::chrono::steady_clock::time_point access_time;
    };
    
    std::unordered_map<std::string, Entry> cache_;
    std::list<std::string> lru_;
    std::unordered_map<std::string, std::list<std::string>::iterator> lru_map_;
    
    size_t capacity_;
    mutable std::shared_mutex mtx_;
    
    // Statistics
    mutable std::atomic<size_t> hits_{0};
    mutable std::atomic<size_t> misses_{0};
    
    void evict_lru();
    bool is_expired(const Entry& entry) const;
    void update_lru_on_access(const std::string& key);
};
