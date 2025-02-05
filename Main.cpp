#include "AppMain.h"
#include <csignal>
#include <iostream>

static void signalHandler(int signum)
{
    std::cerr << "Interrupt: " << signum << std::endl;
    exit(signum);
}

int main(int argc, char* argv[])
{
    std::signal(SIGINT, signalHandler);
    std::signal(SIGSEGV, signalHandler);
    std::signal(SIGTERM, signalHandler);
    std::signal(SIGABRT, signalHandler);

    std::string config_file(
#ifndef NDEBUG
        "VehicleRunner.json"
#else
        "/usr/local/etc/VehicleRunner.json"
#endif
    );

    if (argc >= 2)
        config_file = argv[1];

#ifndef NDEBUG
    std::cout << "Config File: " << config_file << std::endl;
#endif

    AppMain app(config_file);
    return app.Exec();
}

// EOF
