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
            LlmClient(const string& model = "qwen2.5-coder:7b", const string& serverEndpoint = "localhost");
            string generateResponse(const string& prompt);
        };

    } 
} 
