#pragma once
#include <chrono>
#include <atomic>
#include <string>

class CircuitBreaker {
public:
    enum State { CLOSED, OPEN, HALF_OPEN };
    
    CircuitBreaker(int failure_threshold, int timeout_ms);
    
    bool allow_request();
    void record_success();
    void record_failure();
    
    State get_state() const { return state_; }
    std::string get_state_string() const;
    int get_failure_count() const { return failure_count_; }
    
private:
    std::atomic<State> state_;
    std::atomic<int> failure_count_;
    std::atomic<int> success_count_;
    
    const int failure_threshold_;
    const int timeout_ms_;
    std::chrono::steady_clock::time_point last_failure_time_;
    
    void transition_to_open();
    void transition_to_half_open(); 
    void transition_to_closed();
    bool is_timeout_expired() const;
};