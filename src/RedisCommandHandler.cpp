#include "../header_files/RedisCommandHandler.h"

#include <vector>
#include <sstream>
#include <algorithm>
#include <exception>
#include <iostream> // debug
#include <thread>

// RESP parser:
// *2\r\n$4\r\n\PING\r\n$4\r\nTEST\r\n
// *2 -> array has 2 elements
// $4 -> next string has 4 characters
// PING
// TEST


std::vector<std::string> parseRespCommand(const std::string &input) {
    std::vector<std::string> tokens;
    if (input.empty()) return tokens;

    // Inline fallback
    if (input[0] != '*') {
        std::istringstream iss(input);
        std::string tok;
        while (iss >> tok) tokens.push_back(tok);
        return tokens;
    }

    size_t pos = 0;
    if (input[pos] != '*') return tokens;
    ++pos;

    auto find_crlf = [&](size_t p) { return input.find("\r\n", p); };
    size_t crlf = find_crlf(pos);
    if (crlf == std::string::npos) return tokens;

    // parse numElements safely
    int numElements = 0;
    try {
        std::string numStr = input.substr(pos, crlf - pos);
        long tmp = std::stol(numStr);
        if (tmp < 0 || tmp > std::numeric_limits<int>::max()) return tokens;
        numElements = static_cast<int>(tmp);
    } catch (...) { return tokens; }

    pos = crlf + 2;

    for (int i = 0; i < numElements; ++i) {
        if (pos >= input.size() || input[pos] != '$') return tokens;
        ++pos;
        crlf = find_crlf(pos);
        if (crlf == std::string::npos) return tokens;

        int len = 0;
        try {
            std::string lenStr = input.substr(pos, crlf - pos);
            long tmp = std::stol(lenStr);
            if (tmp < 0 || tmp > static_cast<long>(input.size())) return tokens;
            len = static_cast<int>(tmp);
        } catch (...) { return tokens; }

        pos = crlf + 2;

        // Ensure payload + trailing CRLF is inside buffer
        if (pos + static_cast<size_t>(len) + 2 > input.size()) return tokens;

        tokens.emplace_back(input.substr(pos, len));
        pos += static_cast<size_t>(len) + 2; // skip data + CRLF
    }

    return tokens;
}

RedisCommandHandler::RedisCommandHandler() {}

std::string RedisCommandHandler::processCommand(const std::string& commandLine) {
    // Use RESP parser
    auto tokens = parseRespCommand(commandLine);
    if (tokens.empty()) return "-Error: Empty command\r\n";

    std::string cmd = tokens[0];
    std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::toupper);
    // for(auto it : tokens) std::cout << it << std::endl;
    return "";
}