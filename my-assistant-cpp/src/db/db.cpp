#include "db/db.h"
#include <iostream>

namespace assistant {
    namespace database {

        Database_Manager::Database_Manager(const string& dbPath)
            : db(dbPath, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE) 
        {
            try {
                // Create the chat_history table if it doesn't exist
                db.exec(
                    "CREATE TABLE IF NOT EXISTS chat_history ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "prompt TEXT, "
                    "response TEXT, "
                    "timestamp DATETIME DEFAULT CURRENT_TIMESTAMP)"
                );
            } catch (std::exception& e) {
                std::cerr << "SQLite exception during initialization: " << e.what() << std::endl;
            }
        }

        void Database_Manager::saveResponse(const string& prompt, const string& response) {
            try {
                // Prepare a SQL statement to insert safely (prevents SQL injection)
                SQLite::Statement query(db, "INSERT INTO chat_history (prompt, response) VALUES (?, ?)");
                
                // Bind the variables to the ? placeholders
                query.bind(1, prompt);
                query.bind(2, response);
                
                // Execute the statement
                query.exec();
            } catch (std::exception& e) {
                std::cerr << "SQLite exception during saveResponse: " << e.what() << std::endl;
            }
        }

    }
}
