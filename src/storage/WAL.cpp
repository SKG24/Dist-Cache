#include "WAL.h"
#include <iostream>
#include <sstream>

WAL::WAL(const std::string& filename) : filename_(filename) {
    ensure_file_open();
}

WAL::~WAL() {
    if (wal_file_.is_open()) {
        wal_file_.close();
    }
}

void WAL::ensure_file_open() {
    if (!wal_file_.is_open()) {
        wal_file_.open(filename_, std::ios::app);
        if (!wal_file_.is_open()) {
            throw std::runtime_error("Failed to open WAL file: " + filename_);
        }
    }
}

void WAL::append(const std::string& operation, const std::string& key, const std::string& value) {
    std::lock_guard<std::mutex> lock(wal_mutex_);
    ensure_file_open();
    
    wal_file_ << operation << " " << key;
    if (!value.empty()) {
        wal_file_ << " " << value;
    }
    wal_file_ << "\n";
    wal_file_.flush();
}

void WAL::append_binary(const std::string& op, const std::string& key, const std::string& value) {
    // Fallback to text format if msgpack is not available
    #ifdef HAVE_MSGPACK
    std::lock_guard<std::mutex> lock(wal_mutex_);
    ensure_file_open();
    
    std::stringstream buffer;
    msgpack::pack(buffer, std::make_tuple(op, key, value));
    std::string data = buffer.str();
    
    wal_file_.write(data.data(), data.size());
    wal_file_.flush();
    #else
    // Use text format as fallback
    append(op, key, value);
    #endif
}

std::vector<std::tuple<std::string, std::string, std::string>> WAL::replay() {
    std::ifstream in(filename_);
    std::vector<std::tuple<std::string, std::string, std::string>> ops;
    
    if (!in.is_open()) {
        return ops; // Return empty vector if file doesn't exist
    }
    
    std::string line;
    while (std::getline(in, line)) {
        if (line.empty()) continue;
        
        std::istringstream iss(line);
        std::string op, key, value;
        
        iss >> op >> key;
        // Get the rest of the line as value (may contain spaces)
        std::getline(iss, value);
        
        // Trim leading space from value
        if (!value.empty() && value[0] == ' ') {
            value = value.substr(1);
        }
        
        ops.emplace_back(op, key, value);
    }
    
    return ops;
}

void WAL::sync() {
    std::lock_guard<std::mutex> lock(wal_mutex_);
    if (wal_file_.is_open()) {
        wal_file_.flush();
    }
}

void WAL::truncate() {
    std::lock_guard<std::mutex> lock(wal_mutex_);
    wal_file_.close();
    wal_file_.open(filename_, std::ios::trunc);
}