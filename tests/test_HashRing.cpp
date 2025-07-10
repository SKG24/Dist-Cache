#include <gtest/gtest.h>
#include "cluster/HashRing.h"

class HashRingTest : public ::testing::Test {
protected:
    void SetUp() override {
        ring.add_node("node1");
        ring.add_node("node2");
        ring.add_node("node3");
    }
    
    HashRing ring;
};

TEST_F(HashRingTest, BasicDistribution) {
    std::string node1 = ring.get_node("key1");
    std::string node2 = ring.get_node("key2");
    
    EXPECT_FALSE(node1.empty());
    EXPECT_FALSE(node2.empty());
}

TEST_F(HashRingTest, ConsistentMapping) {
    std::string node1 = ring.get_node("test_key");
    std::string node2 = ring.get_node("test_key");
    
    EXPECT_EQ(node1, node2);
}

TEST_F(HashRingTest, NodeRemoval) {
    std::string original_node = ring.get_node("test_key");
    ring.remove_node("node1");
    std::string new_node = ring.get_node("test_key");
    
    // Key should still map to a node
    EXPECT_FALSE(new_node.empty());
}

TEST_F(HashRingTest, LoadDistribution) {
    std::map<std::string, int> distribution;
    
    // Test with many keys
    for (int i = 0; i < 1000; ++i) {
        std::string key = "key" + std::to_string(i);
        std::string node = ring.get_node(key);
        distribution[node]++;
    }
    
    // Each node should get some keys (rough distribution)
    EXPECT_GT(distribution.size(), 1);
    for (const auto& [node, count] : distribution) {
        EXPECT_GT(count, 100);  // At least 10% of keys
        EXPECT_LT(count, 700);  // At most 70% of keys
    }
}