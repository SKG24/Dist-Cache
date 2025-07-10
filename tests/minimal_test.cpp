#include <iostream>
#include <cassert>
#include <string>
#include <chrono>
#include <thread>

// Include core components for basic testing
#include "storage/LRUCache.h"
#include "cluster/HashRing.h"
#include "patterns/CircuitBreaker.h"
#include "storage/MMapPersistence.h"
#include "monitoring/MetricsCollector.h"

// Simple test framework
class MinimalTest {
public:
    static int tests_run;
    static int tests_passed;
    
    static void assert_true(bool condition, const std::string& test_name) {
        tests_run++;
        if (condition) {
            tests_passed++;
            std::cout << "[  PASS  ] " << test_name << std::endl;
        } else {
            std::cout << "[  FAIL  ] " << test_name << std::endl;
        }
    }
    
    static void print_summary() {
        std::cout << "\n=== Test Summary ===" << std::endl;
        std::cout << "Tests run: " << tests_run << std::endl;
        std::cout << "Passed: " << tests_passed << std::endl;
        std::cout << "Failed: " << (tests_run - tests_passed) << std::endl;
        
        if (tests_passed == tests_run) {
            std::cout << "🎉 All tests passed! System is ready for production." << std::endl;
        } else {
            std::cout << "❌ Some tests failed. Please review the implementation." << std::endl;
        }
    }
};

int MinimalTest::tests_run = 0;
int MinimalTest::tests_passed = 0;

// Basic functionality tests
void test_lru_cache() {
    std::cout << "\n--- LRU Cache Tests ---" << std::endl;
    
    LRUCache cache(3);
    
    // Test basic set/get
    cache.set("key1", "value1");
    std::string value;
    bool found = cache.get("key1", value);
    MinimalTest::assert_true(found && value == "value1", "Basic set/get");
    
    // Test non-existent key
    found = cache.get("nonexistent", value);
    MinimalTest::assert_true(!found, "Non-existent key returns false");
    
    // Test capacity limit
    cache.set("key2", "value2");
    cache.set("key3", "value3");
    cache.set("key4", "value4"); // Should evict key1
    
    found = cache.get("key1", value);
    MinimalTest::assert_true(!found, "LRU eviction works");
    
    found = cache.get("key4", value);
    MinimalTest::assert_true(found && value == "value4", "New key accessible after eviction");
}

void test_hash_ring() {
    std::cout << "\n--- Hash Ring Tests ---" << std::endl;
    
    HashRing ring;
    ring.add_node("node1");
    ring.add_node("node2");
    ring.add_node("node3");
    
    // Test consistent mapping
    std::string node1 = ring.get_node("test_key");
    std::string node2 = ring.get_node("test_key");
    MinimalTest::assert_true(node1 == node2, "Consistent key mapping");
    
    // Test node removal
    std::string original_node = ring.get_node("another_key");
    ring.remove_node("node1");
    std::string new_node = ring.get_node("another_key");
    MinimalTest::assert_true(!new_node.empty(), "Key maps to node after removal");
    
    // Test all nodes list
    auto nodes = ring.get_all_nodes();
    MinimalTest::assert_true(nodes.size() == 2, "Correct node count after removal");
}

void test_circuit_breaker() {
    std::cout << "\n--- Circuit Breaker Tests ---" << std::endl;
    
    CircuitBreaker cb(2, 100); // 2 failures, 100ms timeout
    
    // Test initial state
    MinimalTest::assert_true(cb.get_state() == CircuitBreaker::CLOSED, "Initial state is CLOSED");
    MinimalTest::assert_true(cb.allow_request(), "Allows requests when CLOSED");
    
    // Test opening after failures
    cb.record_failure();
    cb.record_failure();
    MinimalTest::assert_true(cb.get_state() == CircuitBreaker::OPEN, "Opens after failures");
    MinimalTest::assert_true(!cb.allow_request(), "Blocks requests when OPEN");
    
    // Test half-open after timeout
    std::this_thread::sleep_for(std::chrono::milliseconds(150));
    MinimalTest::assert_true(cb.allow_request(), "Allows request after timeout");
    MinimalTest::assert_true(cb.get_state() == CircuitBreaker::HALF_OPEN, "Transitions to HALF_OPEN");
    
    // Test closing after success
    cb.record_success();
    MinimalTest::assert_true(cb.get_state() == CircuitBreaker::CLOSED, "Closes after success");
}

void test_persistence() {
    std::cout << "\n--- Persistence Tests ---" << std::endl;
    
    MMapPersistence persistence("test_minimal.dat");
    
    // Test snapshot and load
    std::unordered_map<std::string, std::string> data = {
        {"key1", "value1"},
        {"key2", "value2"}
    };
    
    persistence.snapshot(data);
    auto loaded = persistence.load();
    
    MinimalTest::assert_true(loaded.size() == 2, "Correct number of items loaded");
    MinimalTest::assert_true(loaded["key1"] == "value1", "Correct value loaded for key1");
    MinimalTest::assert_true(loaded["key2"] == "value2", "Correct value loaded for key2");
    
    // Cleanup
    std::remove("test_minimal.dat");
}

void test_metrics() {
    std::cout << "\n--- Metrics Tests ---" << std::endl;
    
    MetricsCollector metrics;
    
    // Test latency recording
    metrics.record_latency(10.5);
    metrics.record_latency(20.0);
    
    // Test counter increment
    metrics.increment_counter("test_ops");
    metrics.increment_counter("test_ops");
    
    // Test JSON generation
    std::string json = metrics.generate_json();
    MinimalTest::assert_true(!json.empty(), "JSON generation works");
    MinimalTest::assert_true(json.find("avg_latency") != std::string::npos, "JSON contains latency");
    MinimalTest::assert_true(json.find("test_ops") != std::string::npos, "JSON contains counter");
}

int main() {
    std::cout << "=== DistCache Minimal Test Suite ===" << std::endl;
    std::cout << "Running basic functionality tests..." << std::endl;
    
    try {
        test_lru_cache();
        test_hash_ring();
        test_circuit_breaker();
        test_persistence();
        test_metrics();
        
        MinimalTest::print_summary();
        
        return (MinimalTest::tests_passed == MinimalTest::tests_run) ? 0 : 1;
    } catch (const std::exception& e) {
        std::cerr << "Test execution failed: " << e.what() << std::endl;
        return 1;
    }
}