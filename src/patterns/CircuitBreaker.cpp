#include "CircuitBreaker.h"
#include <iostream>

CircuitBreaker::CircuitBreaker(int failure_threshold, int timeout_ms) 
    : state_(CLOSED), failure_count_(0), success_count_(0),
      failure_threshold_(failure_threshold), timeout_ms_(timeout_ms) {}

bool CircuitBreaker::allow_request() {
    State current_state = state_.load();
    
    switch (current_state) {
        case CLOSED:
            return true;
            
        case OPEN:
            if (is_timeout_expired()) {
                transition_to_half_open();
                return true;
            }
            return false;
            
        case HALF_OPEN:
            return true;
            
        default:
            return false;
    }
}

void CircuitBreaker::record_success() {
    success_count_++;
    
    if (state_ == HALF_OPEN) {
        transition_to_closed();
    }
}

void CircuitBreaker::record_failure() {
    failure_count_++;
    last_failure_time_ = std::chrono::steady_clock::now();
    
    if (state_ == CLOSED && failure_count_ >= failure_threshold_) {
        transition_to_open();
    } else if (state_ == HALF_OPEN) {
        transition_to_open();
    }
}

void CircuitBreaker::transition_to_open() {
    state_ = OPEN;
    std::cout << "[CircuitBreaker] State: CLOSED -> OPEN (failures: " 
              << failure_count_ << ")\n";
}

void CircuitBreaker::transition_to_half_open() {
    state_ = HALF_OPEN;
    std::cout << "[CircuitBreaker] State: OPEN -> HALF_OPEN (timeout expired)\n";
}

void CircuitBreaker::transition_to_closed() {
    state_ = CLOSED;
    failure_count_ = 0;
    std::cout << "[CircuitBreaker] State: HALF_OPEN -> CLOSED (success recorded)\n";
}

bool CircuitBreaker::is_timeout_expired() const {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        now - last_failure_time_).count();
    return elapsed >= timeout_ms_;
}

std::string CircuitBreaker::get_state_string() const {
    switch (state_) {
        case CLOSED: return "CLOSED";
        case OPEN: return "OPEN"; 
        case HALF_OPEN: return "HALF_OPEN";
        default: return "UNKNOWN";
    }
}
