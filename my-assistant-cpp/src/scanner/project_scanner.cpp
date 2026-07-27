

#include"scanner\project_scanner.h"

#include<filesystem>
#include<vector>
#include<iostream>
namespace fs = std::filesystem;

using namespace std;

namespace assistant{
    namespace scanner{
        vector<string> Scanner::Project_Scanner(const string& filepath){
            vector<string> foundfiles;
            
            IgnoreParser parser;
            parser.loadIgnonreFile(filepath +"/.gitignore");
            try {
                auto it = fs::recursive_directory_iterator(filepath);
                auto end = fs::recursive_directory_iterator();

                while (it != end) {
                    auto& entry = *it;
                    string pathstr = entry.path().string();
                    string filename = entry.path().filename().string();

                    // Phase 1 Filtering: Check if it's a directory we should skip
                    if (entry.is_directory() && !filename.empty() && filename[0] == '.') {
                        it.disable_recursion_pending();
                    }
                    // Skip common build/dependency folders
                    else if (parser.isIgnored(pathstr)) {
                        if (entry.is_directory()){
                            it.disable_recursion_pending();
                        }
                    } 
                    // If it's a regular file, add it to our list
                    else if (entry.is_regular_file()) {
                        foundfiles.push_back(entry.path().string());
                    }

                    // Move to the next item
                    ++it;
                }
            } catch (const std::exception& e) {
                std::cerr << "Scanner Error: Invalid path or permission denied (" << e.what() << ")\n";
            }
            
            return foundfiles;

        }
    }

}