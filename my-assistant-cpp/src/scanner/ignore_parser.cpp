// .gitignore style pattern matching
#include "scanner/project_scanner.h"
#include <iostream>
#include<fstream>
#include<regex>


using namespace std;

namespace assistant{
    namespace scanner{
        void IgnoreParser::loadIgnonreFile(const string& filepath){

            ifstream file(filepath);

            if(!file.is_open()){
                return;
            }

            string line ;
            
            while(getline(file, line)){
                if(line.empty() || line[0] == '#'){
                    continue;
                }

                patterns.push_back(line);
            }

            file.close();
        }

        bool IgnoreParser :: isIgnored(const string& path){
            for(const string& rule : patterns){
                string regex_Rule = rule;
                size_t pos = 0;
                while((pos = regex_Rule.find(".",pos)) !=string:: npos){
                    regex_Rule.replace(pos , 1 , ".*");
                    pos+=2;
                }

                regex patterns(regex_Rule);

                if (regex_search(path, patterns)) {
                    return true;
                }
            }
            return false;
        }
    }
}