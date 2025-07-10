#include "MMapPersistence.h"
#include <iostream>
#include <filesystem>
#include <future>
#include <thread>

MMapPersistence::MMapPersistence(const std::string& filename) : filename_(filename) {
    ensure_directory_exists();
}

void MMapPersistence::snapshot(const std::unordered_map<std::string, std::string>& data) {
    std::ofstream out(filename_, std::ios::trunc);
    if (!out.is_open()) {
        throw std::runtime_error("Failed to open file for writing: " + filename_);
    }
    
    for (const auto& [key, value] : data) {
        // Escape special characters to handle spaces and newlines
        std::string escaped_key = escape_string(key);
        std::string escaped_value = escape_string(value);
        out << escaped_key << " " << escaped_value << "\n";
    }
    
    out.flush();
    if (out.fail()) {
        throw std::runtime_error("Failed to write to file: " + filename_);
    }
}

std::unordered_map<std::string, std::string> MMapPersistence::load() {
    std::unordered_map<std::string, std::string> data;
    
    std::ifstream in(filename_);
    if (!in.is_open()) {
        // File doesn't exist or can't be opened - return empty map
        return data;
    }
    
    std::string line;
    while (std::getline(in, line)) {
        if (line.empty()) continue;
        
        // Find the first space to separate key and value
        size_t space_pos = line.find(' ');
        if (space_pos == std::string::npos) {
            // Invalid line format - skip
            continue;
        }
        
        std::string escaped_key = line.substr(0, space_pos);
        std::string escaped_value = line.substr(space_pos + 1);
        
        // Unescape the strings
        std::string key = unescape_string(escaped_key);
        std::string value = unescape_string(escaped_value);
        
        data[key] = value;
    }
    
    return data;
}

void MMapPersistence::async_snapshot(const std::unordered_map<std::string, std::string>& data) {
    // Create a copy of the data for async processing
    auto data_copy = std::make_shared<std::unordered_map<std::string, std::string>>(data);
    
    std::thread([this, data_copy]() {
        try {
            snapshot(*data_copy);
            std::cout << "[Persistence] Async snapshot completed for " << filename_ << "\n";
        } catch (const std::exception& e) {
            std::cerr << "[Persistence] Async snapshot failed: " << e.what() << "\n";
        }
    }).detach();
}

bool MMapPersistence::file_exists() const {
    std::ifstream file(filename_);
    return file.good();
}

size_t MMapPersistence::get_file_size() const {
    try {
        return std::filesystem::file_size(filename_);
    } catch (const std::filesystem::filesystem_error&) {
        return 0;
    }
}

void MMapPersistence::backup_file(const std::string& backup_filename) {
    try {
        std::filesystem::copy_file(filename_, backup_filename, 
                                   std::filesystem::copy_options::overwrite_existing);
        std::cout << "[Persistence] Backup created: " << backup_filename << "\n";
    } catch (const std::filesystem::filesystem_error& e) {
        throw std::runtime_error("Failed to create backup: " + std::string(e.what()));
    }
}

void MMapPersistence::ensure_directory_exists() {
    try {
        std::filesystem::path file_path(filename_);
        if (file_path.has_parent_path()) {
            std::filesystem::create_directories(file_path.parent_path());
        }
    } catch (const std::filesystem::filesystem_error& e) {
        throw std::runtime_error("Failed to create directory: " + std::string(e.what()));
    }
}

std::string MMapPersistence::escape_string(const std::string& str) const {
    std::string escaped;
    escaped.reserve(str.length() * 2); // Reserve space for worst case
    
    for (char c : str) {
        switch (c) {
            case ' ':
                escaped += "\\s";
                break;
            case '\n':
                escaped += "\\n";
                break;
            case '\r':
                escaped += "\\r";
                break;
            case '\t':
                escaped += "\\t";
                break;
            case '\\':
                escaped += "\\\\";
                break;
            default:
                escaped += c;
                break;
        }
    }
    
    return escaped;
}

std::string MMapPersistence::unescape_string(const std::string& str) const {
    std::string unescaped;
    unescaped.reserve(str.length());
    
    for (size_t i = 0; i < str.length(); ++i) {
        if (str[i] == '\\' && i + 1 < str.length()) {
            switch (str[i + 1]) {
                case 's':
                    unescaped += ' ';
                    ++i; // Skip the next character
                    break;
                case 'n':
                    unescaped += '\n';
                    ++i;
                    break;
                case 'r':
                    unescaped += '\r';
                    ++i;
                    break;
                case 't':
                    unescaped += '\t';
                    ++i;
                    break;
                case '\\':
                    unescaped += '\\';
                    ++i;
                    break;
                default:
                    unescaped += str[i];
                    break;
            }
        } else {
            unescaped += str[i];
        }
    }
    
    return unescaped;
}