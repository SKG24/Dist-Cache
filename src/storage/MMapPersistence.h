#pragma once
#include <string>
#include <unordered_map>
#include <fstream>
#include <stdexcept>

class MMapPersistence {
public:
    explicit MMapPersistence(const std::string& filename);
    
    void snapshot(const std::unordered_map<std::string, std::string>& data);
    std::unordered_map<std::string, std::string> load();
    
    // Enhanced functionality
    void async_snapshot(const std::unordered_map<std::string, std::string>& data);
    bool file_exists() const;
    size_t get_file_size() const;
    void backup_file(const std::string& backup_filename);
    
private:
    std::string filename_;
    
    void ensure_directory_exists();
    std::string escape_string(const std::string& str) const;
    std::string unescape_string(const std::string& str) const;
};