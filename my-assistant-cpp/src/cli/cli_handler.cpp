#include "cli/cli_handler.h"
#include <iostream>
#include <string>
#include "context/file_reader.h"
#include "scanner/project_scanner.h"

using namespace std;

namespace assistant {
    namespace cli {

        void CliHandler::startRepl() {
            string userInput;
            cout << "Assistant started. Type 'exit' to quit.\n";

            while (true) {
                cout << "\n> ";
            
                if (!getline(cin, userInput)) {    // getline helps to read input until the enter key pressed ->  getline(cin, variable)
                    break; 
                }

                // Check if the user wants to exit
                if (userInput == "exit" || userInput == "quit") {
                    cout << "Thanks forreaching us\n";
                    break;
                }

                if (userInput.find("/read ") == 0)
                {
                    string filepath = userInput.substr(6);
                    assistant::context::fileReader reader;
            
                    cout<< reader.readtextfile(filepath)<<endl;
                    continue;
                }

        
                if (userInput.find("/scan ")){
                    string folderpath = userInput.substr(6);
                    // Create the scanner object
                    assistant::scanner::Scanner myScanner;
            
           
                    vector<string> files = myScanner.Project_Scanner(folderpath);
            
            
                    cout << "Found " << files.size() << " files:\n";
            
            
                    for (const string& file : files) {
                        cout << " - " << file << "\n";
                    }
            
                    continue;
        
                }
        

                if (userInput.empty()) {
                    continue; // Ignore empty inputs
                }

                string response = "Assistant: I heard you say \"" + userInput + "\"";
                db.saveResponse(userInput, response);
                cout << response << "\n";
            }
        }

    }
}
