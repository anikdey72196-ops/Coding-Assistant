#pragma once
#include <string>
#include "db/db.h"

namespace assistant {
    namespace cli {

        class CliHandler {
            private:
            assistant::database::Database_Manager db;
            public:
            void startRepl();
        };

    } // namespace cli
} // namespace assistant
