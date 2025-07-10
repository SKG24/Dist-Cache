#pragma once
#include <string>
#include <vector>
#include <tuple>
#include <mutex>
#include <fstream>
#include <sstream>

#ifdef HAVE_MSGPACK
#include <msgpack.hpp>
#endif

class WAL {
public:
    explicit WAL(const std::string& filename);
    ~WAL();
    
    void append(const std::string& operation, const std::string& key, 
                const std::string& value = "");
    void append_binary(const std::string& operation, const std::string& key, 
                      const std::string& value = "");
    std::vector<std::tuple<std::string, std::string, std::string>> replay();
    
    void sync();
    void truncate();
    
private:
    std::string filename_;
    std::ofstream wal_file_;
    std::mutex wal_mutex_;
    
    void ensure_file_open();
};