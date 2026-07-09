#include <iostream>
#include "cli/cli_handler.h"

using namespace std;

int main(int argc, char* argv[]) {
    cout << "My Assistant C++ initialized." << endl;
    
    assistant::cli::CliHandler cli;
    cli.startRepl();
    
    return 0;
}
