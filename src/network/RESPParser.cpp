#include "RESPParser.h"

std::tuple<std::string, std::vector<std::string>> RESPParser::parse(const std::string& raw) {
    if (raw.empty()) {
        return {"", {}};
    }
    
    // Handle different RESP types
    if (raw[0] == '*') {
        return parse_array(raw);
    } else if (raw[0] == '$') {
        return parse_bulk_string(raw);
    } else {
        return parse_simple_string(raw);
    }
}

std::tuple<std::string, std::vector<std::string>> RESPParser::parse_simple_string(const std::string& raw) {
    std::vector<std::string> tokens = split(trim(raw));
    
    if (tokens.empty()) {
        return {"", {}};
    }
    
    std::string command = to_upper(tokens[0]);
    std::vector<std::string> args;
    
    for (size_t i = 1; i < tokens.size(); ++i) {
        args.push_back(tokens[i]);
    }
    
    return {command, args};
}

std::tuple<std::string, std::vector<std::string>> RESPParser::parse_bulk_string(const std::string& raw) {
    // Simplified bulk string parsing for demonstration
    // In production, this would handle the full RESP protocol
    return parse_simple_string(raw.substr(1)); // Skip the '$' prefix
}

std::tuple<std::string, std::vector<std::string>> RESPParser::parse_array(const std::string& raw) {
    // Simplified array parsing for demonstration
    // In production, this would handle the full RESP protocol
    return parse_simple_string(raw.substr(1)); // Skip the '*' prefix
}

std::string RESPParser::serialize(const std::string& result) {
    return "+" + result + "\r\n";
}

std::string RESPParser::serialize_error(const std::string& error) {
    return "-" + error + "\r\n";
}

std::string RESPParser::serialize_integer(int value) {
    return ":" + std::to_string(value) + "\r\n";
}

std::string RESPParser::serialize_bulk(const std::string& data) {
    return "$" + std::to_string(data.length()) + "\r\n" + data + "\r\n";
}

std::string RESPParser::serialize_nil() {
    return "$-1\r\n";
}

std::string RESPParser::serialize_array(const std::vector<std::string>& items) {
    std::string result = "*" + std::to_string(items.size()) + "\r\n";
    for (const auto& item : items) {
        result += serialize_bulk(item);
    }
    return result;
}

std::string RESPParser::to_upper(const std::string& str) {
    std::string upper_str = str;
    std::transform(upper_str.begin(), upper_str.end(), upper_str.begin(), ::toupper);
    return upper_str;
}

std::vector<std::string> RESPParser::split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    
    while (std::getline(ss, token, delimiter)) {
        token = trim(token);
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }
    
    return tokens;
}

std::string RESPParser::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) {
        return "";
    }
    
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}