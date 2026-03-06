#pragma once
#include "constants.hpp"
#include "utils.hpp"
#include <set>
#include <thread>
#include <chrono>
#include <map>

using IncludeCache = std::map<std::string, std::string>;

inline void cmd_init() {
    if (fs::exists(DOCGEN_DIR) || fs::exists(DOCFILE)) {
        std::cerr << "Error: .docgen/ or Docfile already exists." << std::endl;
        return;
    }

    // Create Docfile
    std::ofstream docfile(DOCFILE);
    docfile << "Track:\n    # Example: *.cpp\n\nIgnore:\n    # Example: secret.cpp\n\nStyle:\n    # Example: no emojis\n    # Example: be concise\n";
    docfile.close();

    // Create .docgen directory
    fs::create_directory(DOCGEN_DIR);
    fs::create_directory(DOCS_DIR);

    // Create config
    json config = {
        {"mode", "local"},
        {"cloud", {
            {"api_key", "your_api_key"},
            {"api_url", "https://apifreellm.com/api/v1/chat"},
            {"model_id", "qwen2.5-coder:7b"},
            {"protocol", "simple"}
        }},
        {"local", {
            {"api_url", "http://localhost:11434/api/generate"},
            {"model_id", "qwen2.5-coder:3b"}
        }}
    };
    std::ofstream config_file(CONFIG_FILE);
    config_file << config.dump(4);
    config_file.close();

    // Create lockfile
    std::ofstream lock_file(LOCK_FILE);
    lock_file << "{}";
    lock_file.close();

    std::cout << "Documentation repository initiated in " << fs::current_path().string() << std::endl;
}

inline void cmd_config(int argc, char* argv[]) {
    if (argc < 3) {
        std::cout << "Usage: docgen config <key> <value>\nKeys: mode, protocol, key, model" << std::endl;
        std::cout << "       docgen config see" << std::endl;
        return;
    }
    std::string key = argv[2];

    if (key == "see") {
        if (!fs::exists(CONFIG_FILE)) {
            std::cerr << "Error: Config file not found. Run 'docgen init'." << std::endl;
            return;
        }
        std::ifstream i(CONFIG_FILE);
        json config;
        i >> config;
        std::cout << config.dump(4) << std::endl;
        return;
    }

    if (argc < 4) {
        std::cout << "Usage: docgen config <key> <value>\nKeys: mode\nValues: local, cloud" << std::endl;
        return;
    }
    std::string value = argv[3];

    if (!fs::exists(CONFIG_FILE)) {
        std::cerr << "Error: Config file not found. Run 'docgen init'." << std::endl;
        return;
    }

    std::ifstream i(CONFIG_FILE);
    json config;
    i >> config;
    i.close();

    if (key == "mode") {
        if (value == "cloud" || value == "local") {
            config["mode"] = value;
            std::ofstream o(CONFIG_FILE);
            o << config.dump(4);
            std::cout << "Config updated: mode = " << value << std::endl;
        } else {
            std::cerr << "Error: Invalid mode. Use 'local' or 'cloud'." << std::endl;
        }
    } else if (key == "protocol") {
        config["cloud"]["protocol"] = value;
        std::ofstream o(CONFIG_FILE);
        o << config.dump(4);

        std::cout << "Config updated: cloud.protocol = " << value << std::endl;
    } else {
        std::cerr << "Error: Unknown key '" << key << "'." << std::endl;
    }
}

inline void cmd_track_ignore(const std::string& cmd, const std::string& path) {
    if (!fs::exists(DOCFILE)) {
        std::cerr << "Error: Docfile not found." << std::endl;
        return;
    }
    
    std::vector<std::string> lines;
    std::ifstream f(DOCFILE);
    std::string line;
    bool inserted = false;
    std::string target_header = (cmd == "track") ? "Track:" : "Ignore:";

    while (std::getline(f, line)) {
        lines.push_back(line);
    }
    f.close();

    std::ofstream out(DOCFILE);
    bool in_section = false;
    for (const auto& l : lines) {
        std::string trimmed = trim(l);
        if (trimmed == "Track:" || trimmed == "Ignore:") {
            if (in_section && !inserted) {
                out << "    " << path << "\n";
                inserted = true;
            }
            in_section = (trimmed == target_header);
        }
        out << l << "\n";
    }
    if (in_section && !inserted) {
        out << "    " << path << "\n";
        inserted = true;
    }
    out.close();
    std::cout << "Added " << path << " to " << cmd << " list." << std::endl;
}

inline bool call_ai(const std::string& filepath, const std::string& content, IncludeCache& cache, const std::vector<std::string>& styles, bool verbose = false) {
    if (!fs::exists(CONFIG_FILE)) {
        std::cerr << "Error: Config missing." << std::endl;
        return false;
    }
    std::ifstream i(CONFIG_FILE);
    json config;
    i >> config;

    std::string mode = config.value("mode", "local");
    
    // RAG: Build context from includes
    std::string context_section;
    std::vector<std::string> includes = extract_includes(content);
    fs::path current_path = fs::path(filepath);
    fs::path parent_path = current_path.parent_path();

    for (const auto& inc : includes) {
        // Try relative to current file first, then relative to root
        fs::path p = parent_path / inc;
        if (!fs::exists(p)) {
            p = fs::path(inc);
        }
        
        if (fs::exists(p) && is_text_file(p)) {
            std::string abs_path;
            try {
                abs_path = fs::canonical(p).string();
            } catch (...) {
                abs_path = p.string();
            }
            if (cache.find(abs_path) == cache.end()) {
                cache[abs_path] = read_file(p);
            }
            context_section += "Context from " + inc + ":\n```cpp\n" + cache[abs_path] + "\n```\n\n";
        }
    }

    std::string style_prompt;
    if (!styles.empty()) {
        style_prompt = "\n\nDocumentation Style Guidelines:\n";
        for (const auto& style : styles) {
            style_prompt += "- " + style + "\n";
        }
    }

    std::string prompt = context_section + "Generate Markdown documentation for the following code:\n" + content + style_prompt;
    std::string doc_text;

    int max_retries = 5;
    int wait_time = 1;

    for (int attempt = 0; attempt < max_retries; ++attempt) {
        bool success = false;
        std::string error_msg;
        bool rate_limited = false;

        if (mode == "cloud") {
            std::string api_key = config["cloud"]["api_key"];
            std::string api_url = config["cloud"]["api_url"];
            std::string model_id = config["cloud"]["model_id"];
            std::string protocol = config["cloud"].value("protocol", "openai");

            json body;
            if (protocol == "openai") {
                body = {
                    {"model", model_id},
                    {"messages", {{{"role", "user"}, {"content", prompt}}}}
                };
            } else if (protocol == "google") {
                body = {
                    {"contents", {{{"parts", {{{"text", prompt}}}}}}}
                };
            } else {
                body = {
                    {"model", model_id},
                    {"message", prompt}
                };
            }

            if (verbose) {
                std::cout << "Request to " << api_url << ":\n" << body.dump(4) << std::endl;
            }
            std::string response_str = exec_curl(api_url, {"Authorization: Bearer " + api_key}, body);
            if (verbose) {
                std::cout << "Response:\n" << response_str << std::endl;
            }
            try {
                json response = json::parse(response_str);
                if (response.contains("choices") && !response["choices"].empty()) {
                    doc_text = response["choices"][0]["message"]["content"];
                    success = true;
                } else if (response.contains("candidates") && !response["candidates"].empty()) {
                    doc_text = response["candidates"][0]["content"]["parts"][0]["text"];
                    success = true;
                } else if (response.contains("response")) {
                    doc_text = response["response"];
                    success = true;
                } else {
                    error_msg = "AI Error: " + response_str;
                    if (response.contains("code") && response["code"] == 401) {
                        error_msg += "\nHint: Use 'docgen config key <your_api_key>' to set your API key.";
                    }
                    if (response.contains("retryAfter")) {
                        wait_time = response["retryAfter"].get<int>() + 2;
                        rate_limited = true;
                    }
                }
            } catch (...) {
                error_msg = "Failed to parse AI response.";
            }

        } else { // local
            std::string api_url = config["local"]["api_url"];
            std::string model_id = config["local"]["model_id"];

            json body = {
                {"model", model_id},
                {"prompt", prompt},
                {"stream", false}
            };

            if (verbose) {
                std::cout << "Request to " << api_url << ":\n" << body.dump(4) << std::endl;
            }
            std::string response_str = exec_curl(api_url, {}, body);
            if (verbose) {
                std::cout << "Response:\n" << response_str << std::endl;
            }
            try {
                json response = json::parse(response_str);
                if (response.contains("response")) {
                    doc_text = response["response"];
                    success = true;
                } else {
                    error_msg = "AI Error: " + response_str;
                }
            } catch (...) {
                error_msg = "Failed to parse AI response.";
            }
        }

        if (success) {
            fs::path dest_path = fs::path(DOCS_DIR) / (filepath + ".md");
            write_file(dest_path, doc_text);
            std::cout << "Generated: " << dest_path.string() << std::endl;
            return true;
        }

        std::cerr << error_msg << std::endl;
        if (attempt < max_retries - 1) {
            std::cout << "Retrying in " << wait_time << "s..." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(wait_time));
            if (!rate_limited) {
                wait_time *= 2;
            } else {
                wait_time = 1;
            }
        }
    }

    return false;
}

inline void cmd_update(bool verbose = false) {
    if (!fs::exists(DOCFILE)) {
        std::cerr << "Error: Docfile not found. Run 'docgen init'." << std::endl;
        exit(1);
    }

    json lock_map;
    if (fs::exists(LOCK_FILE)) {
        std::ifstream f(LOCK_FILE);
        if (f.peek() != std::ifstream::traits_type::eof()) {
            f >> lock_map;
        }
    }

    std::vector<std::string> tracks;
    std::vector<std::string> ignores;
    std::vector<std::string> styles;
    std::string current_section;

    std::ifstream f(DOCFILE);
    std::string line;
    while (std::getline(f, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;

        // Handle comments with %
        size_t comment_pos = line.find('%');
        if (comment_pos != std::string::npos) {
            line = trim(line.substr(0, comment_pos));
        }
        if (line.empty()) continue;

        if (line == "Track:") {
            current_section = "track";
            continue;
        }
        if (line == "Ignore:") {
            current_section = "ignore";
            continue;
        }
        if (line == "Style:") {
            current_section = "style";
            continue;
        }

        // Handle "pattern =" syntax if present, though example shows just pattern
        size_t eq_pos = line.find('=');
        std::string pattern = (eq_pos != std::string::npos) ? trim(line.substr(0, eq_pos)) : line;

        if (current_section == "track") tracks.push_back(pattern);
        if (current_section == "ignore") ignores.push_back(pattern);
        if (current_section == "style") styles.push_back(pattern);
    }

    // Collect files
    std::set<fs::path> files_to_process;
    for (const auto& entry : fs::recursive_directory_iterator(".")) {
        if (entry.is_regular_file()) {
            fs::path p = entry.path().lexically_relative(".");
            std::string p_str = p.string();
            
            // Skip .docgen
            if (p_str.find(".docgen") == 0) continue;

            // Check matches
            bool tracked = false;
            for (const auto& t : tracks) {
                if (match_pattern(p, t)) {
                    tracked = true;
                    break;
                }
            }
            if (!tracked) continue;

            bool ignored = false;
            for (const auto& i : ignores) {
                if (match_pattern(p, i)) {
                    ignored = true;
                    break;
                }
            }
            if (ignored) continue;

            if (!is_text_file(p)) continue;

            files_to_process.insert(p);
        }
    }

    // Identify files to update
    struct Task {
        fs::path path;
        std::string path_str;
        std::string hash;
    };
    std::vector<Task> tasks;

    std::cout << "Scanning " << files_to_process.size() << " files..." << std::endl;
    for (const auto& filepath : files_to_process) {
        std::string content = read_file(filepath);
        std::string current_hash = hash_content(content);
        std::string path_str = filepath.string();
        std::replace(path_str.begin(), path_str.end(), '\\', '/'); // Normalize for lockfile

        fs::path dest_path = fs::path(DOCS_DIR) / (path_str + ".md");
        if (!lock_map.contains(path_str) || lock_map[path_str] != current_hash || !fs::exists(dest_path)) {
            tasks.push_back({filepath, path_str, current_hash});
        }
    }

    if (tasks.empty()) {
        std::cout << "All files are up to date." << std::endl;
        return;
    }

    // Process updates
    auto start_time = std::chrono::steady_clock::now();
    size_t total_tasks = tasks.size();

    IncludeCache cache;
    for (size_t i = 0; i < total_tasks; ++i) {
        const auto& task = tasks[i];
        
        std::string eta_msg;
        if (i > 0) {
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - start_time).count();
            if (elapsed > 0) {
                double avg = static_cast<double>(elapsed) / i;
                size_t remaining = static_cast<size_t>(avg * (total_tasks - i));
                eta_msg = " (ETA: " + std::to_string(remaining) + "s)";
            }
        }

        std::cout << "[" << (i + 1) << "/" << total_tasks << "] Updating: " << task.path_str << eta_msg << std::endl;
        
        std::string content = read_file(task.path);
        if (call_ai(task.path_str, content, cache, styles, verbose)) {
            lock_map[task.path_str] = task.hash;
        }
    }

    std::ofstream out(LOCK_FILE);
    out << lock_map.dump(4);
    std::cout << "Update complete." << std::endl;
}

inline void cmd_reboot() {
    std::cout << "This will delete " << DOCFILE << " and " << DOCGEN_DIR << "/. Are you sure? (y/n): ";
    char c;
    std::cin >> c;
    if (c == 'y' || c == 'Y') {
        fs::remove_all(DOCGEN_DIR);
        fs::remove(DOCFILE);
        std::cout << "Reboot complete." << std::endl;
    } else {
        std::cout << "Cancelled." << std::endl;
    }
}

inline void cmd_sponsor() {
    std::cout << "Opening GitHub Sponsors page..." << std::endl;
#ifdef _WIN32
    system("start https://github.com/sponsors/alonsovm44");
#elif __APPLE__
    system("open https://github.com/sponsors/alonsovm44");
#else
    system("xdg-open https://github.com/sponsors/alonsovm44");
#endif
}