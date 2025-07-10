#include <gtest/gtest.h>
#include <thread>
#include <chrono>
#include <vector>
#include "monitoring/MetricsCollector.h"

class MetricsCollectorTest : public ::testing::Test {
protected:
    void SetUp() override {
        metrics = std::make_unique<MetricsCollector>();
    }
    
    void TearDown() override {
        metrics.reset();
    }
    
    std::unique_ptr<MetricsCollector> metrics;
};

TEST_F(MetricsCollectorTest, BasicLatencyRecording) {
    metrics->record_latency(10.5);
    metrics->record_latency(20.3);
    metrics->record_latency(15.7);
    
    std::string json = metrics->generate_json();
    
    // Should contain average latency
    EXPECT_NE(json.find("avg_latency"), std::string::npos);
    EXPECT_NE(json.find("requests"), std::string::npos);
}

TEST_F(MetricsCollectorTest, CounterIncrement) {
    metrics->increment_counter("test_counter");
    metrics->increment_counter("test_counter");
    metrics->increment_counter("another_counter");
    
    std::string json = metrics->generate_json();
    
    EXPECT_NE(json.find("test_counter"), std::string::npos);
    EXPECT_NE(json.find("another_counter"), std::string::npos);
}

TEST_F(MetricsCollectorTest, ConnectionTracking) {
    metrics->record_active_connections(5);
    metrics->record_active_connections(10);
    metrics->record_active_connections(3);
    
    std::string json = metrics->generate_json();
    
    EXPECT_NE(json.find("connections"), std::string::npos);
    EXPECT_NE(json.find("3"), std::string::npos); // Last recorded value
}

TEST_F(MetricsCollectorTest, ConcurrentMetrics) {
    const int num_threads = 4;
    const int ops_per_thread = 1000;
    
    std::vector<std::thread> threads;
    
    for (int t = 0; t < num_threads; ++t) {
        threads.emplace_back([this, t, ops_per_thread]() {
            for (int i = 0; i < ops_per_thread; ++i) {
                metrics->record_latency(1.0 + i % 10);
                metrics->increment_counter("concurrent_ops");
            }
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    std::string json = metrics->generate_json();
    
    // Should have recorded all operations
    EXPECT_NE(json.find("concurrent_ops"), std::string::npos);
    EXPECT_NE(json.find("requests"), std::string::npos);
}

TEST_F(MetricsCollectorTest, JSONFormatValidation) {
    metrics->record_latency(5.5);
    metrics->increment_counter("test_ops");
    metrics->record_active_connections(2);
    
    std::string json = metrics->generate_json();
    
    // Basic JSON structure validation
    EXPECT_EQ(json.front(), '{');
    EXPECT_EQ(json.back(), '}');
    EXPECT_NE(json.find("avg_latency"), std::string::npos);
    EXPECT_NE(json.find("requests"), std::string::npos);
    EXPECT_NE(json.find("connections"), std::string::npos);
    EXPECT_NE(json.find("test_ops"), std::string::npos);
}