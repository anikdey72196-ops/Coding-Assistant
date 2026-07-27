// HTTP client to OpenAI / Anthropic / Ollama
#include "llm/llm_client.h"
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include<httplib.h>
#include<nlohmann/json.hpp>
#include<iostream>

using namespace std;
using json = nlohmann::json;

namespace assistant{
    namespace llm{
        LlmClient::LlmClient(const string& model): endpoint("localhost"),modelName(model){

        }
        string LlmClient::generateResponse(const string& prompt){
            httplib::Client cli(endpoint , 11434);
            
            cli.set_read_timeout(540, 0); 
            cli.set_connection_timeout(15, 0);

            json payload = {
                {"model" , modelName},
                {"prompt", prompt},
                {"stream", true}
            };
            auto res = cli.Post("/api/generate", payload.dump(), "application/json");

    // 4. Handle the response
    if (res && res->status == 200) {
        try {
            // Parse the returned JSON
            json responseJson = json::parse(res->body);
            // Extract the "response" field
            return responseJson["response"].get<std::string>();
        } catch (const std::exception& e) {
            return "Error parsing JSON response: " + std::string(e.what());
        }
    } else {
        std::string errorMsg = "HTTP Request failed. ";
        if (res) {
            errorMsg += "Status code: " + std::to_string(res->status);
        } else {
            auto err = res.error();
            errorMsg += "Error code: " + std::to_string(static_cast<int>(err));
        }
        return errorMsg;
    }
}

        }
    }

    
