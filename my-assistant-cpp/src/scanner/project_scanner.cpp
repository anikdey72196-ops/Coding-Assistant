// Recursive directory walk, .gitignore parsing

#include"scanner\project_scanner.h"

#include<filesystem>
#include<vector>
namespace fs = std::filesystem;

using namespace std;

namespace assistant{
    namespace scanner{
        vector<string> Scanner::Project_Scanner(const string& filepath){
            vector<string> foundfiles;
            
            // Create an iterator pointing to the directory
            auto it = fs::recursive_directory_iterator(filepath);
            // Default constructed iterator acts as the "end" condition
            auto end = fs::recursive_directory_iterator();

            while (it != end) {
                auto& entry = *it;
                string filename = entry.path().filename().string();

                // Phase 1 Filtering: Check if it's a directory we should skip
                if (entry.is_directory()) {
                    // Skip hidden folders (start with '.')
                    if (!filename.empty() && filename[0] == '.') {
                        it.disable_recursion_pending();
                    } 
                    // Skip common build/dependency folders
                    else if (filename == "build" || filename == "node_modules") {
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
            
            return foundfiles;

        }
    }

}