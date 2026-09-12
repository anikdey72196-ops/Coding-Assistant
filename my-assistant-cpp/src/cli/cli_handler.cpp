#include "cli/cli_handler.h"
#include <iostream>
#include <string>
#include <filesystem>
#include "context/file_reader.h"
#include "scanner/project_scanner.h"
#include "code_inspector.h"

using namespace std;
namespace fs = std::filesystem;

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
                    string targetPath = userInput.substr(6);
                    assistant::inspector::CodeInspector inspector;

                    // Inspect and auto-repair target path (file or directory)
                    inspector.inspectPath(targetPath);

                    if (fs::is_regular_file(targetPath)) {
                        assistant::context::fileReader reader;
                        string content = reader.readtextfile(targetPath);
                        cout << content << endl;

                        if (!activeContext.empty()) {
                            activeContext += "\n\n====================\n\n";
                        }
                        activeContext += "File: " + targetPath + "\nContent:\n" + content;
                        cout << "\n[Success: " << targetPath << " added to AI context memory!]\n";
                    } else if (fs::is_directory(targetPath)) {
                        cout << "\n[Success: Inspected directory " << targetPath << "]\n";
                    }
                    continue;
                }

                if (userInput == "/clear") {
                    activeContext.clear();
                    cout << "[Context memory cleared!]\n";
                    continue;
                }

                if (userInput.find("/scan ") == 0) {
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

                cout << "Assistant is thinking...\n";
                
                // Build the full prompt including active file context if present
                string promptToSend = userInput;
                if (!activeContext.empty()) {
                    promptToSend = "Active Context:\n" + activeContext + "\n\nUser Question:\n" + userInput;
                }

                string response = llmClient.generateResponse(promptToSend);
                db.saveResponse(userInput, response);
                cout << "Assistant: " << response << "\n";
            }
        }

    }
}
