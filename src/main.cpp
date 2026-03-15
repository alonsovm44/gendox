#include "core.hpp"

const std::string VERSION = "0.2.1";

void print_help() {
    std::cout << "Usage: docgen <command> [args...]\n\n"
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
              << "  upgrade    Upgrade docgen to the latest version\n"
              << "  reboot     Reset the documentation repository (deletes .docgen/)\n"
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
        std::cout << "docgen version " << VERSION << std::endl;
        return 0;
    } else if (command == "init") {
        cmd_init();
    } else if (command == "config") {
        cmd_config(argc, argv);
    } else if (command == "track") {
        if (argc < 3) {
            std::cout << "Usage: docgen track <path>" << std::endl;
            return 1;
        }
        cmd_track_ignore("track", argv[2]);
    } else if (command == "ignore") {
        if (argc < 3) {
            std::cout << "Usage: docgen ignore <path>" << std::endl;
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
            std::cout << "Usage: docgen query \"<question>\"" << std::endl;
            return 1;
        }
        cmd_query(argv[2]);
    } else if (command == "upgrade") {
        cmd_upgrade();
    } else if (command == "reboot") {
        cmd_reboot();
    } else if (command == "sponsor") {
        cmd_sponsor();
    } else if (command == "get-key") {
        // Simple placeholder for opening browser
#ifdef _WIN32
        system("start https://apifreellm.com");
#elif __APPLE__
        system("open https://apifreellm.com");
#else
        system("xdg-open https://apifreellm.com");
#endif
    } else {
        std::cout << "Unknown command: " << command << "\nRun 'docgen help' for usage." << std::endl;
        return 1;
    }

    return 0;
}