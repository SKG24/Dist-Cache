#pragma once
#include <string>
#include <vector>
#include <tuple>
#include <sstream>
#include <algorithm>
#include <cctype>

class RESPParser {
public:
    // Parse command from client
    std::tuple<std::string, std::vector<std::string>> parse(const std::string& raw);
    
    // Serialize responses
    std::string serialize(const std::string& result);
    std::string serialize_error(const std::string& error);
    std::string serialize_integer(int value);
    std::string serialize_bulk(const std::string& data);
    std::string serialize_nil();
    std::string serialize_array(const std::vector<std::string>& items);
    
    // Utility functions
    static std::string to_upper(const std::string& str);
    static std::vector<std::string> split(const std::string& str, char delimiter = ' ');
    static std::string trim(const std::string& str);
    
private:
    // RESP protocol parsing
    std::tuple<std::string, std::vector<std::string>> parse_simple_string(const std::string& raw);
    std::tuple<std::string, std::vector<std::string>> parse_bulk_string(const std::string& raw);
    std::tuple<std::string, std::vector<std::string>> parse_array(const std::string& raw);
};