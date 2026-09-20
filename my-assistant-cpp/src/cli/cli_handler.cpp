#include "cli/cli_handler.h"
#include <iostream>
#include <string>
#include <filesystem>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#endif
#include "context/file_reader.h"
#include "scanner/project_scanner.h"
#include "code_inspector.h"

using namespace std;
namespace fs = std::filesystem;

static std::string trimString(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

static std::string replaceAll(std::string str, const std::string& from, const std::string& to) {
    if (from.empty()) return str;
    size_t startPos = 0;
    while ((startPos = str.find(from, startPos)) != std::string::npos) {
        str.replace(startPos, from.length(), to);
        startPos += to.length();
    }
    return str;
}

static std::string cleanTerminalText(std::string text) {
    // Replace non-breaking hyphens, fancy dashes, and quotes with standard ASCII equivalents
    text = replaceAll(text, "\xE2\x80\x91", "-");   // Non-breaking hyphen (‑)
    text = replaceAll(text, "\xE2\x80\x93", "-");   // En dash (–)
    text = replaceAll(text, "\xE2\x80\x94", "--");  // Em dash (—)
    text = replaceAll(text, "\xE2\x80\x98", "'");   // Left single quote (‘)
    text = replaceAll(text, "\xE2\x80\x99", "'");   // Right single quote (’)
    text = replaceAll(text, "\xE2\x80\x9C", "\"");  // Left double quote (“)
    text = replaceAll(text, "\xE2\x80\x9D", "\"");  // Right double quote (”)
    text = replaceAll(text, "\xE2\x80\xA2", "*");   // Bullet (•)
    return text;
}

namespace assistant {
    namespace cli {

        void CliHandler::startRepl() {
#ifdef _WIN32
            SetConsoleOutputCP(CP_UTF8);
            SetConsoleCP(CP_UTF8);
#endif
            string userInput;
            cout << "Assistant started. Type 'exit' to quit.\n";

            while (true) {
                cout << "\n> ";
            
                if (!getline(cin, userInput)) {    // getline helps to read input until the enter key pressed ->  getline(cin, variable)
                    break; 
                }

                if (!userInput.empty() && userInput.back() == '\r') {
                    userInput.pop_back();
                }

                std::string trimmedInput = trimString(userInput);

                // Check if the user wants to exit
                if (trimmedInput == "exit" || trimmedInput == "quit") {
                    cout << "Thanks for reaching us\n";
                    break;
                }

                if (trimmedInput.find("/read ") == 0)
                {
                    string targetPath = trimString(trimmedInput.substr(6));
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

                if (trimmedInput == "/clear") {
                    activeContext.clear();
                    cout << "[Context memory cleared!]\n";
                    continue;
                }

                if (trimmedInput.find("/scan ") == 0) {
                    string folderpath = trimString(trimmedInput.substr(6));
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
                cout << "Assistant: " << cleanTerminalText(response) << "\n";
            }
        }

    }
}
