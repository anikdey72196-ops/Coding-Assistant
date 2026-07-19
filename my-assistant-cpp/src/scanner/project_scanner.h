// Project scanner header
#pragma once
#include <string>
#include <vector>

using namespace std;
namespace assistant{
    namespace scanner{
        class Scanner{
            public:
            vector<string> Project_Scanner(const string& filepath);
        };
        class IgnoreParser{
            private:
            vector<string> patterns ; 
            
            public:
            void loadIgnonreFile(const string& filepath);
            bool isIgnored(const string& path);
        };
    }
}