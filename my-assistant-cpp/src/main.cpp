#include <iostream>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#endif
#include "cli/cli_handler.h"

using namespace std;

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
    cout << "My Assistant C++ initialized." << endl;
    
    assistant::cli::CliHandler cli;
    cli.startRepl();
    
    return 0;
}
