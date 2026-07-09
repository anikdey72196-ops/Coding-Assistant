#include <iostream>
#include<sstream>
#include<string>
#include<fstream>
#include "context/file_reader.h"


using namespace std;

namespace assistant{
    namespace context{
        string fileReader :: readtextfile(const string& filepath){
            
            ifstream inputFIle(filepath);

            if (!inputFIle.is_open()){
                cout<<"Error: Could not open the file" + filepath<<endl;
            }
            string line ;
            cout<< "reading file content"<<endl;
           string fullText = ""; 
            while (getline(inputFIle, line)){
                fullText += line + "\n"; // The \n puts the new line back in!
            }

            inputFIle.close();
            return fullText;

        }
    }
}

