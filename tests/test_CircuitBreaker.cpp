#include <gtest/gtest.h>
#include <thread>
#include <chrono>
#include "patterns/CircuitBreaker.h"
#include <vector>

TEST(CircuitBreakerTest, ClosedStateAllowsRequests) {
    CircuitBreaker cb(3, 1000);
    
    EXPECT_EQ(cb.get_state(), CircuitBreaker::CLOSED);
    EXPECT_TRUE(cb.allow_request());
}

TEST(CircuitBreakerTest, OpensAfterFailures) {
    CircuitBreaker cb(2, 1000);
    
    // Record failures
    cb.record_failure();
    EXPECT_TRUE(cb.allow_request());
    
    cb.record_failure();
    EXPECT_FALSE(cb.allow_request());  // Should be OPEN now
    EXPECT_EQ(cb.get_state(), CircuitBreaker::OPEN);
}

TEST(CircuitBreakerTest, TransitionsToHalfOpen) {
    CircuitBreaker cb(1, 100);  // 100ms timeout
    
    cb.record_failure();
    EXPECT_FALSE(cb.allow_request());
    
    // Wait for timeout
    std::this_thread::sleep_for(std::chrono::milliseconds(150));
    
    EXPECT_TRUE(cb.allow_request());
    EXPECT_EQ(cb.get_state(), CircuitBreaker::HALF_OPEN);
}

TEST(CircuitBreakerTest, RecoveryToClosed) {
    CircuitBreaker cb(1, 100);
    
    // Open the circuit
    cb.record_failure();
    EXPECT_EQ(cb.get_state(), CircuitBreaker::OPEN);
    
    // Wait and try again
    std::this_thread::sleep_for(std::chrono::milliseconds(150));
    EXPECT_TRUE(cb.allow_request());
    
    // Record success - should close
    cb.record_success();
    EXPECT_EQ(cb.get_state(), CircuitBreaker::CLOSED);
    EXPECT_EQ(cb.get_failure_count(), 0);
}