#pragma once
#include <string>
#include <SQLiteCpp/SQLiteCpp.h>

using namespace std;

namespace assistant {
    namespace database {
        class Database_Manager {
            private:
                SQLite::Database db;

            public:
                // Pass the path to the database file in the constructor
                Database_Manager(const string& dbPath = "assistant.db" );
                void saveResponse(const string& prompt, const string& response);
                vector<pair<string, string>> getRecentHistory(int limit = 3);
        };
    }
}
