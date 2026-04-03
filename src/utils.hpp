#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <cstdio>
#include <array>
#include <algorithm>
#include <regex>

#include "json.hpp"

using json = nlohmann::json;
namespace fs = std::filesystem;

// Simple FNV-1a hash to avoid external dependencies like OpenSSL
inline std::string hash_content(const std::string& content) {
    uint64_t hash = 14695981039346656037ULL;
    for (char c : content) {
        hash ^= static_cast<unsigned char>(c);
        hash *= 1099511628211ULL;
    }
    std::stringstream ss;
    ss << std::hex << std::setw(16) << std::setfill('0') << hash;
    return ss.str();
}

inline bool is_text_file(const fs::path& path) {
    std::ifstream f(path, std::ios::binary);
    if (!f) return false;
    char buffer[1024];
    f.read(buffer, sizeof(buffer));
    size_t n = f.gcount();
    for (size_t i = 0; i < n; ++i) {
        if (buffer[i] == 0) return false;
    }
    return true;
}

inline std::string read_file(const fs::path& path) {
    std::ifstream f(path, std::ios::binary);
    if (!f) return "";
    std::stringstream buffer;
    buffer << f.rdbuf();
    return buffer.str();
}

inline void write_file(const fs::path& path, const std::string& content) {
    fs::create_directories(path.parent_path());
    std::ofstream f(path, std::ios::binary);
    f << content;
}

inline std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (std::string::npos == first) return str;
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

// Get current git commit hash
inline std::string get_git_commit() {
    std::string result;
    std::array<char, 128> buffer;
#ifdef _WIN32
    FILE* pipe = _popen("git rev-parse HEAD 2>nul", "r");
#else
    FILE* pipe = popen("git rev-parse HEAD 2>/dev/null", "r");
#endif
    if (!pipe) {
        return "unknown";
    }
    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
        result += buffer.data();
    }
#ifdef _WIN32
    _pclose(pipe);
#else
    pclose(pipe);
#endif
    result = trim(result);
    return result.empty() ? "unknown" : result;
}

// Execute a shell command and return output. Uses curl for HTTP.
inline std::string exec_curl(const std::string& url, const std::vector<std::string>& headers, const json& body) {
    // Write body to a temp file to avoid shell escaping hell
    fs::path temp_file = fs::temp_directory_path() / "gendox_req.json";
    {
        std::ofstream ofs(temp_file);
        ofs << body.dump();
    }

    std::string cmd = "curl -s -X POST \"" + url + "\"";
    for (const auto& h : headers) {
        cmd += " -H \"" + h + "\"";
    }
    cmd += " -H \"Content-Type: application/json\"";
    cmd += " -d \"@" + temp_file.string() + "\"";

    std::string result;
    std::array<char, 128> buffer;
#ifdef _WIN32
    FILE* pipe = _popen(cmd.c_str(), "r");
#else
    FILE* pipe = popen(cmd.c_str(), "r");
#endif
    if (!pipe) {
        std::cerr << "Error: Failed to run curl." << std::endl;
        return "";
    }
    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
        result += buffer.data();
    }
#ifdef _WIN32
    _pclose(pipe);
#else
    pclose(pipe);
#endif
    fs::remove(temp_file);
    return result;
}

inline bool match_pattern(const fs::path& path, const std::string& pattern) {
    std::string p_str = path.string();
    // Normalize separators
    std::replace(p_str.begin(), p_str.end(), '\\', '/');
    std::string pat = pattern;
    std::replace(pat.begin(), pat.end(), '\\', '/');

    // Directory match (e.g., "dir/")
    if (pat.back() == '/') {
        return p_str.find(pat) == 0;
    }
    // Extension match (e.g., "*.cpp")
    if (pat.front() == '*') {
        std::string ext = pat.substr(1);
        if (p_str.length() >= ext.length()) {
            return p_str.compare(p_str.length() - ext.length(), ext.length(), ext) == 0;
        }
        return false;
    }
    // Exact match
    return p_str == pat;
}

inline std::vector<std::string> extract_includes(const std::string& content) {
    std::vector<std::string> includes;
    // Regex to match:
    // 1. #include or import with "filename" or <filename> (C/C++/Obj-C/etc.)
    // 2. using Namespace; (C#)
    // 3. from Module import ... (Python) 
    // 4. import Module (Python)
    // 5. package Name; (Java) or package Name (Go)
    std::regex re(R"((?:#\s*include|import)\s*[<"]([^>"]+)[>"]|using\s+([\w\.]+)\s*;|from\s+([\w\.]+)\s+import|import\s+([\w\.]+)|\bpackage\s+([\w\.]+)\s*;?)");
    std::smatch match;
    std::string::const_iterator searchStart(content.cbegin());
    while (std::regex_search(searchStart, content.cend(), match, re)) {
        for (size_t i = 1; i < match.size(); ++i) {
            if (match[i].matched) {
                includes.push_back(match[i].str());
                break;
            }
        }
        searchStart = match.suffix().first;
    }
    return includes;
}