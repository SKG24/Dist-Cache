#pragma once
#include <string>
#include <vector>
#include <tuple>
#include <mutex>
#include <fstream>
#include <sstream>
#include <atomic>
#include <thread>
#include <condition_variable>
#include <queue>
#include <msgpack.hpp>

class WAL {
public:
    struct Operation {
        std::string op;
        std::string key;
        std::string value;
        std::chrono::steady_clock::time_point timestamp;
    };
    
    explicit WAL(const std::string& filename);
    ~WAL();
    
    void append(const std::string& operation, const std::string& key, 
                const std::string& value = "");
    void append_binary(const std::string& operation, const std::string& key, 
                      const std::string& value = "");
    std::vector<std::tuple<std::string, std::string, std::string>> replay();
    
    // Enhanced functionality
    void sync();
    void truncate();
    void start_async_writer();
    void stop_async_writer();
    size_t get_pending_operations() const;
    
private:
    std::string filename_;
    std::ofstream wal_file_;
    std::mutex wal_mutex_;
    
    // Async writing support
    std::queue<Operation> pending_ops_;
    std::thread writer_thread_;
    std::condition_variable cv_;
    std::atomic<bool> async_writer_running_{false};
    
    void ensure_file_open();
    void async_writer_loop();
    void flush_pending_operations();
};
