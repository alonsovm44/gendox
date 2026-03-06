#include "core.hpp"

const std::string VERSION = "0.0.1";

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: docgen <command> [args...]" << std::endl;
        return 1;
    }

    std::string command = argv[1];

    if (command == "version" || command == "-version" || command == "--version") {
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
        std::cout << "Unknown command: " << command << std::endl;
        return 1;
    }

    return 0;
}