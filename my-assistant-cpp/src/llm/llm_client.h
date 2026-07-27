#pragma once
#include <string>
using namespace std;
namespace assistant {
    namespace llm {

        class LlmClient {
            private:
            string endpoint;
            string modelName;

            public:
            LlmClient(const string& model = "gemma4:12b");
            string generateResponse(const string& prompt);
        };

    } 
} 
