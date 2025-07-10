#include <gtest/gtest.h>
#include <thread>
#include <vector>
#include <chrono>
#include "storage/LRUCache.h"

class LRUCacheTest : public ::testing::Test {
protected:
    void SetUp() override {
        cache = std::make_unique<LRUCache>(5);
    }
    
    void TearDown() override {
        cache.reset();
    }
    
    std::unique_ptr<LRUCache> cache;
};

TEST_F(LRUCacheTest, BasicSetAndGet) {
    cache->set("key1", "value1");
    
    std::string value;
    bool found = cache->get("key1", value);
    
    EXPECT_TRUE(found);
    EXPECT_EQ(value, "value1");
}

TEST_F(LRUCacheTest, GetNonExistentKey) {
    std::string value;
    bool found = cache->get("nonexistent", value);
    
    EXPECT_FALSE(found);
}

TEST_F(LRUCacheTest, SetAndDelete) {
    cache->set("key1", "value1");
    EXPECT_TRUE(cache->exists("key1"));
    
    cache->del("key1");
    EXPECT_FALSE(cache->exists("key1"));
}

TEST_F(LRUCacheTest, TTLExpiration) {
    cache->set("temp_key", "temp_value", 1); // 1 second TTL
    
    std::string value;
    EXPECT_TRUE(cache->get("temp_key", value));
    EXPECT_EQ(value, "temp_value");
    
    // Wait for expiration
    std::this_thread::sleep_for(std::chrono::milliseconds(1100));
    
    EXPECT_FALSE(cache->get("temp_key", value));
    EXPECT_FALSE(cache->exists("temp_key"));
}

TEST_F(LRUCacheTest, LRUEviction) {
    // Fill cache to capacity
    for (int i = 0; i < 5; ++i) {
        cache->set("key" + std::to_string(i), "value" + std::to_string(i));
    }
    
    // Add one more - should evict least recently used
    cache->set("key5", "value5");
    
    // key0 should be evicted
    std::string value;
    EXPECT_FALSE(cache->get("key0", value));
    EXPECT_TRUE(cache->get("key1", value));
    EXPECT_TRUE(cache->get("key5", value));
}

TEST_F(LRUCacheTest, LRUOrdering) {
    cache->set("a", "1");
    cache->set("b", "2");
    cache->set("c", "3");
    
    // Access 'a' to make it more recently used
    std::string value;
    cache->get("a", value);
    
    // Fill remaining capacity
    cache->set("d", "4");
    cache->set("e", "5");
    
    // Add one more - should evict 'b' (least recently used)
    cache->set("f", "6");
    
    EXPECT_TRUE(cache->get("a", value));  // Should still exist
    EXPECT_FALSE(cache->get("b", value)); // Should be evicted
    EXPECT_TRUE(cache->get("c", value));  // Should still exist
}

TEST_F(LRUCacheTest, ConcurrentAccess) {
    const int num_threads = 4;
    const int ops_per_thread = 1000;
    
    std::vector<std::thread> threads;
    
    for (int t = 0; t < num_threads; ++t) {
        threads.emplace_back([this, t, ops_per_thread]() {
            for (int i = 0; i < ops_per_thread; ++i) {
                std::string key = "thread" + std::to_string(t) + "_key" + std::to_string(i);
                std::string value = "thread" + std::to_string(t) + "_value" + std::to_string(i);
                
                cache->set(key, value);
                
                std::string retrieved_value;
                if (cache->get(key, retrieved_value)) {
                    EXPECT_EQ(retrieved_value, value);
                }
            }
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
}

TEST_F(LRUCacheTest, SizeAndCapacity) {
    EXPECT_EQ(cache->size(), 0);
    EXPECT_EQ(cache->capacity(), 5);
    
    cache->set("key1", "value1");
    EXPECT_EQ(cache->size(), 1);
    
    // Fill to capacity
    for (int i = 1; i < 5; ++i) {
        cache->set("key" + std::to_string(i), "value" + std::to_string(i));
    }
    EXPECT_EQ(cache->size(), 5);
    
    // Add one more - size should remain at capacity
    cache->set("key6", "value6");
    EXPECT_EQ(cache->size(), 5);
}

TEST_F(LRUCacheTest, HitRateTracking) {
    cache->set("key1", "value1");
    cache->set("key2", "value2");
    
    std::string value;
    cache->get("key1", value);  // Hit
    cache->get("key2", value);  // Hit
    cache->get("key3", value);  // Miss
    
    double hit_rate = cache->hit_rate();
    EXPECT_DOUBLE_EQ(hit_rate, 2.0/3.0); // 2 hits out of 3 attempts
}
