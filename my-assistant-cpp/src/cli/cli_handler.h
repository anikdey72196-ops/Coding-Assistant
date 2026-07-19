#pragma once
#include <string>
#include "db/db.h"
#include "llm/llm_client.h"

namespace assistant {
    namespace cli {

        class CliHandler {
            private:
            assistant::database::Database_Manager db;
            assistant::llm::LlmClient llmClient;
            public:
            void startRepl();
        };

    } // namespace cli
} // namespace assistant
