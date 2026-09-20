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
            LlmClient(const string& model = "gpt-oss:20b-cloud", const string& serverEndpoint = "localhost");
            string generateResponse(const string& prompt);
        };

    } 
} 
