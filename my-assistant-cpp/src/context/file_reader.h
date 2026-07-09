// File reader header
#pragma once
#include<iostream>
#include<string>
 
using namespace std;

namespace assistant{
    namespace context{
        class fileReader{
            public:
            string readtextfile(const string& filepath);
            
        };
    }
}