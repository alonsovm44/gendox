#include "core.hpp"

const std::string VERSION = "0.2.2";

void open_url(const std::string& url) {
#ifdef _WIN32
    system(("start " + url).c_str());
#elif __APPLE__
    system(("open " + url).c_str());
#else
    system(("xdg-open " + url).c_str());
#endif
}

void print_help() {
    std::cout << "Usage: gendox <command> [args...]\n\n"
              << "Commands:\n"
              << "  init       Initialize a new documentation project\n"
              << "  config     Manage configuration (AI mode, keys, models)\n"
              << "  track      Track a file or directory for documentation\n"
              << "  ignore     Ignore a file or directory\n"
              << "  update     Generate or update documentation for tracked files\n"
              << "  auto       Automatically update documentation when files change\n"
              << "  status     Show status of tracked files (new/modified)\n"
              << "  validate   Check if documentation is up-to-date\n"
              << "  clean      Remove documentation for untracked files\n"
              << "  graph      Generate dependency graph (DOT format)\n"
              << "  query      Ask a question about the codebase\n"
              << "  summary    Generate a summary of the documentation status\n"
              << "  upgrade    Upgrade gendox to the latest version\n"
              << "  reboot     Reset the documentation repository (deletes .gendox/)\n"
              << "  sponsor    Open the GitHub Sponsors page\n"
              << "  get-key    Open browser to get a free API key\n"
              << "  version    Display the current version\n"
              << "  help       Display this help message\n"
              << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        print_help();
        return 0;
    }

    std::string command = argv[1];

    if (command == "help" || command == "-h" || command == "--help") {
        print_help();
        return 0;
    } else if (command == "version" || command == "-version" || command == "--version") {
        std::cout << "gendox version " << VERSION << std::endl;
        return 0;
    } else if (command == "init") {
        cmd_init();
    } else if (command == "config") {
        cmd_config(argc, argv);
    } else if (command == "track") {
        if (argc < 3) {
            std::cout << "Usage: gendox track <path>" << std::endl;
            return 1;
        }
        cmd_track_ignore("track", argv[2]);
    } else if (command == "ignore") {
        if (argc < 3) {
            std::cout << "Usage: gendox ignore <path>" << std::endl;
            return 1;
        }
        cmd_track_ignore("ignore", argv[2]);
    } else if (command == "update") {
        bool verbose = false;
        if (argc > 2 && (std::string(argv[2]) == "-v" || std::string(argv[2]) == "--verbose")) {
            verbose = true;
        }
        cmd_update(verbose);
    } else if (command == "auto") {
        cmd_auto();
    } else if (command == "summary") {
        cmd_summary();
    } else if (command == "status") {
        cmd_status();
    } else if (command == "clean") {
        cmd_clean();
    } else if (command == "validate") {
        cmd_validate();
    } else if (command == "graph") {
        cmd_graph();
    } else if (command == "query") {
        if (argc < 3) {
            std::cout << "Usage: gendox query \"<question>\"" << std::endl;
            return 1;
        }
        cmd_query(argv[2]);
    } else if (command == "upgrade") {
        cmd_upgrade();
    } else if (command == "reboot") {
        cmd_reboot();
    } else if (command == "sponsor") {
        open_url("https://github.com/sponsors/alonsovm44");
    } else if (command == "get-key") {
        open_url("https://apifreellm.com");
    } else {
        std::cout << "Unknown command: " << command << "\nRun 'gendox help' for usage." << std::endl;
        return 1;
    }

    return 0;
}