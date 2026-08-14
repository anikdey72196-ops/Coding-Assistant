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
            LlmClient(const string& model = "gemma4:26b", const string& serverEndpoint = "localhost");
            string generateResponse(const string& prompt);
        };

    } 
} 
