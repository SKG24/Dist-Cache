#include "LRUCache.h"
#include <iostream>

LRUCache::LRUCache(size_t capacity) : capacity_(capacity) {}

bool LRUCache::get(const std::string& key, std::string& value) {
    std::shared_lock lock(mtx_);
    
    auto it = cache_.find(key);
    if (it == cache_.end()) {
        misses_++;
        return false;
    }
    
    if (is_expired(it->second)) {
        misses_++;
        return false;
    }
    
    // Update access time for LRU
    it->second.access_time = std::chrono::steady_clock::now();
    value = it->second.value;
    hits_++;
    return true;
}

void LRUCache::set(const std::string& key, const std::string& value, int ttl_seconds) {
    std::unique_lock lock(mtx_);
    
    auto now = std::chrono::steady_clock::now();
    Entry entry{
        value,
        ttl_seconds > 0 ? now + std::chrono::seconds(ttl_seconds) : now + std::chrono::hours(24),
        now
    };
    
    // Remove from LRU if exists
    if (lru_map_.count(key)) {
        lru_.erase(lru_map_[key]);
    }
    
    cache_[key] = entry;
    lru_.push_front(key);
    lru_map_[key] = lru_.begin();
    
    // Evict if over capacity
    while (cache_.size() > capacity_) {
        evict_lru();
    }
}

void LRUCache::del(const std::string& key) {
    std::unique_lock lock(mtx_);
    
    if (cache_.count(key)) {
        lru_.erase(lru_map_[key]);
        lru_map_.erase(key);
        cache_.erase(key);
    }
}

bool LRUCache::exists(const std::string& key) {
    std::shared_lock lock(mtx_);
    auto it = cache_.find(key);
    return it != cache_.end() && !is_expired(it->second);
}

void LRUCache::cleanup_expired() {
    std::unique_lock lock(mtx_);
    
    auto now = std::chrono::steady_clock::now();
    auto it = cache_.begin();
    
    while (it != cache_.end()) {
        if (it->second.expire_time < now) {
            lru_.erase(lru_map_[it->first]);
            lru_map_.erase(it->first);
            it = cache_.erase(it);
        } else {
            ++it;
        }
    }
}

size_t LRUCache::size() const {
    std::shared_lock lock(mtx_);
    return cache_.size();
}

double LRUCache::hit_rate() const {
    size_t total_hits = hits_.load();
    size_t total_misses = misses_.load();
    size_t total = total_hits + total_misses;
    return total > 0 ? (double)total_hits / total : 0.0;
}

void LRUCache::evict_lru() {
    if (lru_.empty()) return;
    
    std::string old_key = lru_.back();
    lru_.pop_back();
    cache_.erase(old_key);
    lru_map_.erase(old_key);
}

bool LRUCache::is_expired(const Entry& entry) const {
    return entry.expire_time < std::chrono::steady_clock::now();
}