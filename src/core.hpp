#pragma once
#include "constants.hpp"
#include "utils.hpp"
#include <set>
#include <thread>
#include <chrono>
#include <map>
#include <ctime>

using IncludeCache = std::map<std::string, std::string>;

struct DocfileConfig {
    std::vector<std::string> tracks;
    std::vector<std::string> ignores;
    std::vector<std::string> styles;
};

inline DocfileConfig parse_docfile() {
    DocfileConfig config;
    if (!fs::exists(DOCFILE)) return config;

    std::ifstream f(DOCFILE);
    std::string line;
    std::string current_section;
    while (std::getline(f, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;

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

        size_t eq_pos = line.find('=');
        std::string pattern = (eq_pos != std::string::npos) ? trim(line.substr(0, eq_pos)) : line;

        if (current_section == "track") config.tracks.push_back(pattern);
        if (current_section == "ignore") config.ignores.push_back(pattern);
        if (current_section == "style") config.styles.push_back(pattern);
    }
    return config;
}

inline std::set<fs::path> scan_files(const DocfileConfig& config) {
    bool ignore_hidden = false;
    if (fs::exists(CONFIG_FILE)) {
        std::ifstream i(CONFIG_FILE);
        json j;
        if (i.peek() != std::ifstream::traits_type::eof()) {
            i >> j;
            ignore_hidden = j.value("ignore_hidden", false);
        }
    }

    std::set<fs::path> files;
    for (const auto& entry : fs::recursive_directory_iterator(".")) {
        if (entry.is_regular_file()) {
            fs::path p = entry.path().lexically_relative(".");
            std::string p_str = p.string();
            
            if (p_str.find(".docgen") == 0) continue;

            if (ignore_hidden) {
                bool hidden = false;
                for (const auto& part : p) {
                    std::string part_str = part.string();
                    if (!part_str.empty() && part_str[0] == '.' && part_str != "." && part_str != "..") {
                        hidden = true;
                        break;
                    }
                }
                if (hidden) continue;
            }

            bool tracked = false;
            for (const auto& t : config.tracks) {
                if (match_pattern(p, t)) {
                    tracked = true;
                    break;
                }
            }
            if (!tracked) continue;

            bool ignored = false;
            for (const auto& i : config.ignores) {
                if (match_pattern(p, i)) {
                    ignored = true;
                    break;
                }
            }
            if (ignored) continue;

            if (!is_text_file(p)) continue;

            // Mitigate prompt fatigue: skip extremely large files
            // 50KB is roughly ~12k-15k tokens.
            if (fs::file_size(p) > 50000) {
                continue;
            }

            files.insert(p);
        }
    }
    return files;
}

inline void cmd_init() {
    if (fs::exists(DOCGEN_DIR) || fs::exists(DOCFILE)) {
        std::cerr << "Error: .docgen/ or Docfile already exists." << std::endl;
        return;
    }

    std::string project_name;
    std::cout << "Enter project name: ";
    std::getline(std::cin, project_name);
    project_name = trim(project_name);

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
        {"ignore_hidden", false},
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

    // Create tree.json
    json tree_data;
    tree_data["project_name"] = project_name;
    tree_data["files"] = json::array();
    std::ofstream tree_file(DOCGEN_DIR + "/tree.json");
    tree_file << tree_data.dump(4);
    tree_file.close();

    std::cout << "Documentation repository initiated in " << fs::current_path().string() << std::endl;
}

inline void cmd_config(int argc, char* argv[]) {
    if (argc < 3) {
        std::cout << "Usage: docgen config <key> <value>\nKeys: mode, protocol, key, model, ignore-hidden" << std::endl;
        std::cout << "       docgen config see" << std::endl;
        std::cout << "       docgen config check" << std::endl;
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

    if (key == "check") {
        if (!fs::exists(CONFIG_FILE)) {
            std::cerr << "Error: Config file not found. Run 'docgen init'." << std::endl;
            return;
        }
        std::ifstream i(CONFIG_FILE);
        json config;
        i >> config;

        std::string mode = config.value("mode", "local");
        std::cout << "Checking connection for mode: " << mode << "..." << std::endl;

        std::string api_url;
        std::string api_key;
        std::string model_id;
        std::string protocol;

        if (mode == "cloud") {
            api_url = config["cloud"]["api_url"];
            api_key = config["cloud"]["api_key"];
            model_id = config["cloud"]["model_id"];
            protocol = config["cloud"].value("protocol", "simple");
        } else {
            api_url = config["local"]["api_url"];
            model_id = config["local"]["model_id"];
            protocol = "simple";
        }

        std::string prompt = "Test connection. Reply with 'OK'.";
        json body;
        if (protocol == "openai") {
            body = {{"model", model_id}, {"messages", {{{"role", "user"}, {"content", prompt}}}}};
        } else if (protocol == "google") {
            body = {{"contents", {{{"parts", {{{"text", prompt}}}}}}}};
        } else {
            body = {{"model", model_id}, {"message", prompt}};
        }

        std::vector<std::string> headers;
        if (!api_key.empty() && api_key != "your_api_key") {
            headers.push_back("Authorization: Bearer " + api_key);
        }

        std::cout << "Sending request to " << api_url << "..." << std::endl;
        std::string response_str = exec_curl(api_url, headers, body);

        try {
            json response = json::parse(response_str);
            if (response.contains("error") || (response.contains("code") && response["code"].is_number() && response["code"].get<int>() >= 400)) {
                std::cerr << "Connection failed: " << response.dump(4) << std::endl;
            } else {
                std::cout << "Connection successful!" << std::endl;
            }
        } catch (...) {
            std::cerr << "Connection failed. Invalid JSON response: " << response_str << std::endl;
        }
        return;
    }

    if (argc < 4) {
        std::cout << "Usage: docgen config <key> <value>\nKeys: mode, protocol, key, model, ignore-hidden" << std::endl;
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
    } else if (key == "ignore-hidden") {
        bool val = (value == "true" || value == "1");
        config["ignore_hidden"] = val;
        std::ofstream o(CONFIG_FILE);
        o << config.dump(4);
        std::cout << "Config updated: ignore_hidden = " << (val ? "true" : "false") << std::endl;
    } else if (key == "protocol") {
        config["cloud"]["protocol"] = value;
        std::ofstream o(CONFIG_FILE);
        o << config.dump(4);

        std::cout << "Config updated: cloud.protocol = " << value << std::endl;
    } else if (key == "key") {
        config["cloud"]["api_key"] = value;
        std::ofstream o(CONFIG_FILE);
        o << config.dump(4);

        std::cout << "Config updated: cloud.api_key = " << value << std::endl;
    } else if (key == "model") {
        std::string current_mode = config.value("mode", "local");
        if (current_mode == "cloud") {
            config["cloud"]["model_id"] = value;
            std::cout << "Config updated: cloud.model_id = " << value << std::endl;
        } else {
            config["local"]["model_id"] = value;
            std::cout << "Config updated: local.model_id = " << value << std::endl;
        }
        std::ofstream o(CONFIG_FILE);
        o << config.dump(4);
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

inline bool call_ai(const std::string& filepath, const std::string& content, IncludeCache& cache, const std::vector<std::string>& styles, std::string& out_doc, bool verbose = false) {
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

    const size_t MAX_CONTEXT_CHARS = 12000; // Approx 3000 tokens to prevent prompt fatigue
    bool context_truncated = false;

    for (const auto& inc : includes) {
        if (context_section.size() >= MAX_CONTEXT_CHARS) {
            context_truncated = true;
            break;
        }
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
            
            if (context_section.size() + cache[abs_path].size() <= MAX_CONTEXT_CHARS) {
                context_section += "Context from " + inc + ":\n```cpp\n" + cache[abs_path] + "\n```\n\n";
            } else {
                context_truncated = true;
            }
        }
    }
    if (context_truncated) {
        context_section += "\n> Note: Some context files were omitted to fit within the prompt limit.\n\n";
    }

    std::string style_prompt;
    if (!styles.empty()) {
        style_prompt = "\n\nDocumentation Style Guidelines:\n";
        for (const auto& style : styles) {
            style_prompt += "- " + style + "\n";
        }
    }

    fs::path dest_path = fs::path(DOCS_DIR) / (filepath + ".md");
    std::string existing_doc;
    if (fs::exists(dest_path)) {
        existing_doc = read_file(dest_path);
    }

    std::string prompt;
    if (existing_doc.empty()) {
        prompt = context_section + 
            "Generate comprehensive Markdown documentation for the following code.\n"
            "Focus on the purpose, usage, and behavior. Avoid stating the obvious.\n"
            "Code:\n" + content + style_prompt;
    } else {
        prompt = context_section + 
            "Update the following documentation based on the new code.\n"
            "Only modify what has changed in the code. Preserve the existing structure and content where possible.\n"
            "Existing Documentation:\n" + existing_doc + "\n\n"
            "New Code:\n" + content + style_prompt;
    }
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
            std::string protocol = config["cloud"].value("protocol", "simple");

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
                    if ((response.contains("code") && response["code"] == 429) || 
                        (response.contains("error") && response["error"].contains("code") && 
                         (response["error"]["code"] == 429 || response["error"]["code"] == "429"))) {
                        error_msg += "\nRate limit exceeded (429).";
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
            write_file(dest_path, doc_text);
            out_doc = doc_text;
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

inline void cmd_update(bool verbose = false, bool auto_mode = false) {
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

    // Load tree.json
    json tree_data;
    fs::path tree_path = fs::path(DOCGEN_DIR) / "tree.json";
    if (fs::exists(tree_path)) {
        std::ifstream f(tree_path);
        if (f.peek() != std::ifstream::traits_type::eof()) {
            f >> tree_data;
        }
    }
    if (!tree_data.contains("files") || !tree_data["files"].is_array()) {
        tree_data["files"] = json::array();
    }
    if (!tree_data.contains("project_name") && tree_data.contains("project")) {
        tree_data["project_name"] = tree_data["project"];
        tree_data.erase("project");
    }

    DocfileConfig config = parse_docfile();
    std::set<fs::path> files_to_process = scan_files(config);

    // Identify files to update
    struct Task {
        fs::path path;
        std::string path_str;
        std::string hash;
    };
    std::vector<Task> tasks;

    if (!auto_mode) {
        std::cout << "Scanning " << files_to_process.size() << " files..." << std::endl;
    }
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
        if (!auto_mode) {
            std::cout << "All files are up to date." << std::endl;
        }
        return;
    }
    if (auto_mode) {
        std::cout << "Changes detected. Updating..." << std::endl;
    }

    // Process updates
    auto start_time = std::chrono::steady_clock::now();
    size_t total_tasks = tasks.size();

    IncludeCache cache;
    bool tree_updated = false;
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
        std::string doc_content;
        if (call_ai(task.path_str, content, cache, config.styles, doc_content, verbose)) {
            lock_map[task.path_str] = task.hash;

            // Update tree.json entry
            std::vector<std::string> deps = extract_includes(content);
            
            std::string summary;
            std::stringstream ss(doc_content);
            std::string line;
            while(std::getline(ss, line)) {
                line = trim(line);
                if(line.empty()) continue;
                if(line[0] == '#') continue;
                summary = line;
                break;
            }
            if (summary.empty()) summary = "No summary available.";

            std::string doc_path_str = (fs::path(DOCS_DIR) / (task.path_str + ".md")).string();
            std::replace(doc_path_str.begin(), doc_path_str.end(), '\\', '/');

            json file_entry = {
                {"path", task.path_str},
                {"doc_path", doc_path_str},
                {"hash", task.hash},
                {"dependencies", deps},
                {"summary", summary}
            };

            bool found = false;
            for (auto& item : tree_data["files"]) {
                if (item["path"] == task.path_str) {
                    item = file_entry;
                    found = true;
                    break;
                }
            }
            if (!found) {
                tree_data["files"].push_back(file_entry);
            }
            tree_updated = true;
        }
    }

    std::ofstream out(LOCK_FILE);
    out << lock_map.dump(4);

    if (tree_updated) {
        auto now = std::chrono::system_clock::now();
        std::time_t now_c = std::chrono::system_clock::to_time_t(now);
        char buf[30];
        std::strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%SZ", std::gmtime(&now_c));
        tree_data["last_update"] = buf;

        std::ofstream tree_out(tree_path);
        tree_out << tree_data.dump(4);
    }

    std::cout << "Update complete." << std::endl;
}

inline void cmd_auto() {
    std::cout << "Docgen Auto-Update Mode" << std::endl;
    std::cout << "Watching tracked files for changes... (Ctrl+C to stop)" << std::endl;
    
    std::map<std::string, fs::file_time_type> last_write_times;

    while (true) {
        DocfileConfig config = parse_docfile();
        std::set<fs::path> files = scan_files(config);
        bool changed = false;

        for (const auto& p : files) {
            try {
                auto current_time = fs::last_write_time(p);
                std::string path_str = p.string();
                if (last_write_times.find(path_str) == last_write_times.end() || last_write_times[path_str] != current_time) {
                    last_write_times[path_str] = current_time;
                    changed = true;
                }
            } catch (...) { continue; }
        }

        if (changed) {
            cmd_update(false, true);
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

inline void cmd_summary() {
    fs::path tree_path = fs::path(DOCGEN_DIR) / "tree.json";
    if (!fs::exists(tree_path)) {
        std::cerr << "Error: tree.json not found. Run 'docgen update' first." << std::endl;
        return;
    }

    std::ifstream f(tree_path);
    json tree_data;
    try {
        f >> tree_data;
    } catch (const std::exception& e) {
        std::cerr << "Error: Failed to parse tree.json: " << e.what() << std::endl;
        return;
    }

    std::string project_name = tree_data.value("project_name", "Project");
    std::stringstream ss;
    ss << "# " << project_name << " Documentation Summary\n\n";
    
    if (tree_data.contains("last_update")) {
        ss << "Last updated: " << tree_data["last_update"].get<std::string>() << "\n\n";
    }

    if (tree_data.contains("files") && tree_data["files"].is_array()) {
        std::vector<json> files;
        for (const auto& item : tree_data["files"]) {
            files.push_back(item);
        }
        
        std::sort(files.begin(), files.end(), [](const json& a, const json& b) {
            return a.value("path", "") < b.value("path", "");
        });

        for (const auto& file : files) {
            std::string path = file.value("path", "unknown");
            std::string summary = file.value("summary", "No summary available.");
            std::string doc_path = file.value("doc_path", "");
            
            ss << "## " << path << "\n\n";
            ss << summary << "\n\n";
            if (!doc_path.empty()) {
                std::replace(doc_path.begin(), doc_path.end(), '\\', '/');
                ss << "[View full documentation](" << doc_path << ")\n\n";
            }
            ss << "---\n\n";
        }
    }

    std::string output_file = "SUMMARY.md";
    std::ofstream out(output_file);
    out << ss.str();
    out.close();

    std::cout << "Summary generated: " << output_file << std::endl;
}

inline void cmd_status() {
    if (!fs::exists(DOCFILE)) {
        std::cerr << "Error: Docfile not found. Run 'docgen init'." << std::endl;
        return;
    }

    json lock_map;
    if (fs::exists(LOCK_FILE)) {
        std::ifstream f(LOCK_FILE);
        if (f.peek() != std::ifstream::traits_type::eof()) {
            f >> lock_map;
        }
    }

    DocfileConfig config = parse_docfile();
    std::set<fs::path> files = scan_files(config);

    std::cout << "Project Status:\n" << std::endl;
    bool changes = false;

    for (const auto& p : files) {
            std::string path_str = p.string();
            std::replace(path_str.begin(), path_str.end(), '\\', '/');

            std::string content = read_file(p);
            std::string current_hash = hash_content(content);

            if (!lock_map.contains(path_str)) {
                std::cout << " [NEW]      " << path_str << std::endl;
                changes = true;
            } else if (lock_map[path_str] != current_hash) {
                std::cout << " [MODIFIED] " << path_str << std::endl;
                changes = true;
            }
    }

    if (!changes) {
        std::cout << "Everything up to date." << std::endl;
    }
}

inline void cmd_clean() {
    if (!fs::exists(DOCFILE)) {
        std::cerr << "Error: Docfile not found. Run 'docgen init'." << std::endl;
        return;
    }

    DocfileConfig config = parse_docfile();
    std::set<fs::path> files = scan_files(config);

    // Identify currently tracked files
    std::set<std::string> tracked_files;
    for (const auto& p : files) {
            std::string path_str = p.string();
            std::replace(path_str.begin(), path_str.end(), '\\', '/');
            tracked_files.insert(path_str);
    }

    // Load tree.json
    json tree_data;
    fs::path tree_path = fs::path(DOCGEN_DIR) / "tree.json";
    if (fs::exists(tree_path)) {
        std::ifstream f_tree(tree_path);
        if (f_tree.peek() != std::ifstream::traits_type::eof()) {
            f_tree >> tree_data;
        }
    }

    // Load lockfile
    json lock_map;
    if (fs::exists(LOCK_FILE)) {
        std::ifstream f_lock(LOCK_FILE);
        if (f_lock.peek() != std::ifstream::traits_type::eof()) {
            f_lock >> lock_map;
        }
    }

    bool changes = false;
    if (tree_data.contains("files") && tree_data["files"].is_array()) {
        auto& files = tree_data["files"];
        for (auto it = files.begin(); it != files.end(); ) {
            std::string path = (*it)["path"];
            
            if (tracked_files.find(path) == tracked_files.end()) {
                std::cout << "Removing untracked file: " << path << std::endl;
                
                // Delete doc file
                if ((*it).contains("doc_path")) {
                    std::string doc_path_str = (*it)["doc_path"];
                    fs::path doc_path = doc_path_str;
                    if (fs::exists(doc_path)) {
                        fs::remove(doc_path);
                        std::cout << "  Deleted: " << doc_path_str << std::endl;
                    }
                }

                // Remove from lockfile
                if (lock_map.contains(path)) {
                    lock_map.erase(path);
                }

                it = files.erase(it);
                changes = true;
            } else {
                ++it;
            }
        }
    }

    if (changes) {
        std::ofstream tree_out(tree_path);
        tree_out << tree_data.dump(4);

        std::ofstream lock_out(LOCK_FILE);
        lock_out << lock_map.dump(4);
        std::cout << "Clean complete." << std::endl;
    } else {
        std::cout << "Nothing to clean." << std::endl;
    }
}

inline void cmd_validate() {
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

    DocfileConfig config = parse_docfile();
    std::set<fs::path> files = scan_files(config);

    std::cout << "Validating documentation..." << std::endl;
    bool valid = true;

    for (const auto& p : files) {
            std::string path_str = p.string();
            std::replace(path_str.begin(), path_str.end(), '\\', '/');

            std::string content = read_file(p);
            std::string current_hash = hash_content(content);

            fs::path doc_path = fs::path(DOCS_DIR) / (path_str + ".md");

            if (!lock_map.contains(path_str)) {
                std::cout << " [FAIL] Untracked/New file: " << path_str << std::endl;
                valid = false;
            } else if (lock_map[path_str] != current_hash) {
                std::cout << " [FAIL] Outdated documentation: " << path_str << std::endl;
                valid = false;
            } else if (!fs::exists(doc_path)) {
                std::cout << " [FAIL] Missing documentation file: " << doc_path.string() << std::endl;
                valid = false;
            }
    }

    if (valid) {
        std::cout << "Validation successful." << std::endl;
    } else {
        std::cerr << "Validation failed. Run 'docgen update' to fix." << std::endl;
        exit(1);
    }
}

inline void cmd_graph() {
    fs::path tree_path = fs::path(DOCGEN_DIR) / "tree.json";
    if (!fs::exists(tree_path)) {
        std::cerr << "Error: tree.json not found. Run 'docgen update' first." << std::endl;
        return;
    }

    std::ifstream f(tree_path);
    json tree_data;
    try {
        f >> tree_data;
    } catch (const std::exception& e) {
        std::cerr << "Error: Failed to parse tree.json: " << e.what() << std::endl;
        return;
    }

    std::string project_name = tree_data.value("project_name", "Project");
    
    std::stringstream ss;
    ss << "digraph \"" << project_name << "\" {\n";
    ss << "    node [shape=box, style=filled, fillcolor=\"#e8e8e8\", fontname=\"Helvetica\"];\n";
    ss << "    edge [color=\"#555555\"];\n";
    ss << "    rankdir=LR;\n";
    ss << "    label=\"" << project_name << " Dependency Graph\";\n";
    ss << "    labelloc=\"t\";\n\n";

    if (tree_data.contains("files") && tree_data["files"].is_array()) {
        for (const auto& file : tree_data["files"]) {
            std::string source = file.value("path", "");
            if (source.empty()) continue;

            ss << "    \"" << source << "\";\n";

            if (file.contains("dependencies") && file["dependencies"].is_array()) {
                for (const auto& dep : file["dependencies"]) {
                    std::string target = dep.get<std::string>();
                    ss << "    \"" << source << "\" -> \"" << target << "\";\n";
                }
            }
        }
    }
    ss << "}\n";

    fs::path output_path = fs::path(DOCGEN_DIR) / "graph.dot";
    std::ofstream out(output_path);
    out << ss.str();
    out.close();

    std::cout << "Graph generated: " << output_path.string() << std::endl;
    std::cout << "Visualize with: dot -Tpng " << output_path.string() << " -o graph.png" << std::endl;
}

inline void cmd_query(const std::string& q) {
    std::string question = trim(q);
    bool chat_mode = false;
    
    if (question == "--chat") {
        chat_mode = true;
        question = "";
    } else if (question.find("--chat ") == 0) {
        chat_mode = true;
        question = trim(question.substr(7));
    } else if (question.find(" --chat") != std::string::npos) {
        chat_mode = true;
        question.erase(question.find(" --chat"), 7);
        question = trim(question);
    }

    if (!fs::exists(CONFIG_FILE)) {
        std::cerr << "Error: Config file not found. Run 'docgen init'." << std::endl;
        return;
    }
    
    std::ifstream i(CONFIG_FILE);
    json config;
    i >> config;

    fs::path tree_path = fs::path(DOCGEN_DIR) / "tree.json";
    if (!fs::exists(tree_path)) {
        std::cerr << "Error: tree.json not found. Run 'docgen update' first." << std::endl;
        return;
    }
    std::ifstream f(tree_path);
    json tree_data;
    if (f.peek() != std::ifstream::traits_type::eof()) {
        f >> tree_data;
    }

    std::string context;
    const size_t MAX_CONTEXT_CHARS = 32000; 

    if (tree_data.contains("files") && tree_data["files"].is_array()) {
        for (const auto& file : tree_data["files"]) {
            if (context.size() >= MAX_CONTEXT_CHARS) break;
            
            std::string doc_path_str = file.value("doc_path", "");
            if (doc_path_str.empty()) continue;
            
            fs::path p = doc_path_str;
            if (fs::exists(p)) {
                std::string content = read_file(p);
                std::string path = file.value("path", "unknown");
                if (context.size() + content.size() < MAX_CONTEXT_CHARS) {
                    context += "File: " + path + "\n" + content + "\n\n";
                }
            }
        }
    }

    if (context.empty()) {
        std::cout << "No documentation found. Run 'docgen update' to generate docs." << std::endl;
        return;
    }

    std::string mode = config.value("mode", "local");
    std::string api_url;
    std::string api_key;
    std::string model_id;
    std::string protocol;

    if (mode == "cloud") {
        api_url = config["cloud"]["api_url"];
        api_key = config["cloud"]["api_key"];
        model_id = config["cloud"]["model_id"];
        protocol = config["cloud"].value("protocol", "simple");
    } else {
        api_url = config["local"]["api_url"];
        model_id = config["local"]["model_id"];
        protocol = "simple";
    }

    json openai_messages = json::array();
    std::string text_history = "Context from project documentation:\n" + context + "\n\nAnswer questions based on the context provided.\n\n";

    if (protocol == "openai") {
        openai_messages.push_back({{"role", "system"}, {"content", "You are a helpful coding assistant. Answer the user's questions based on the following context:\n" + context}});
    }

    if (chat_mode) {
        std::cout << "Entering chat mode. Type 'exit' or 'quit' to end.\n";
    }

    while (true) {
        if (chat_mode && question.empty()) {
            std::cout << "\n(docgen) > ";
            std::getline(std::cin, question);
            question = trim(question);
            if (question == "exit" || question == "quit") {
                break;
            }
            if (question.empty()) continue;
        }

        json body;
        if (protocol == "openai") {
            openai_messages.push_back({{"role", "user"}, {"content", question}});
            body = {{"model", model_id}, {"messages", openai_messages}};
        } else if (protocol == "google") {
            text_history += "Question: " + question + "\nAnswer: ";
            body = {{"contents", {{{"parts", {{{"text", text_history}}}}}}}};
        } else {
            text_history += "Question: " + question + "\nAnswer: ";
            if (mode == "local") {
                body = {{"model", model_id}, {"prompt", text_history}, {"stream", false}};
            } else {
                body = {{"model", model_id}, {"message", text_history}};
            }
        }

        std::vector<std::string> headers;
        if (!api_key.empty() && api_key != "your_api_key") {
            headers.push_back("Authorization: Bearer " + api_key);
        }

        std::cout << "Analyzing documentation..." << std::endl;
        std::string response_str = exec_curl(api_url, headers, body);

        try {
            json response = json::parse(response_str);
            std::string answer;
            
            if (response.contains("choices") && !response["choices"].empty()) {
                answer = response["choices"][0]["message"]["content"];
            } else if (response.contains("candidates") && !response["candidates"].empty()) {
                answer = response["candidates"][0]["content"]["parts"][0]["text"];
            } else if (response.contains("response")) {
                answer = response["response"];
            } else {
                std::cerr << "Error: Unexpected API response format.\n" << response.dump(4) << std::endl;
                if (!chat_mode) return;
                question = "";
                continue;
            }
            
            std::cout << "\n" << answer << std::endl;

            if (protocol == "openai") {
                openai_messages.push_back({{"role", "assistant"}, {"content", answer}});
            } else {
                text_history += answer + "\n\n";
            }

        } catch (const std::exception& e) {
            std::cerr << "Error parsing response: " << e.what() << "\nRaw response: " << response_str << std::endl;
        }

        if (!chat_mode) {
            break;
        }
        question = ""; // Reset for next iteration
    }
}

inline void cmd_upgrade() {
    std::cout << "Checking for updates..." << std::endl;
#ifdef _WIN32
    // Execute the PowerShell update script from the master branch
    int result = system("powershell -Command \"irm https://raw.githubusercontent.com/alonsovm44/docgen/master/update.ps1 | iex\"");
#else
    // Execute the Bash update script from the master branch
    int result = system("curl -fsSL https://raw.githubusercontent.com/alonsovm44/docgen/master/update.sh | bash");
#endif
    if (result != 0) std::cerr << "Upgrade failed." << std::endl;
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