#include "task_manager.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace assistant {
namespace tasks {

static std::string escapeCsv(const std::string& field) {
    bool needsQuotes = false;
    for (char c : field) {
        if (c == ',' || c == '"' || c == '\n' || c == '\r') {
            needsQuotes = true;
            break;
        }
    }
    if (!needsQuotes) {
        return field;
    }
    std::string escaped = "\"";
    for (char c : field) {
        if (c == '"') {
            escaped += "\"\"";
        } else {
            escaped += c;
        }
    }
    escaped += "\"";
    return escaped;
}

static std::vector<std::string> parseCsvLine(const std::string& line) {
    std::vector<std::string> tokens;
    std::string token;
    bool inQuotes = false;

    for (size_t i = 0; i < line.size(); ++i) {
        char c = line[i];
        if (c == '"') {
            if (inQuotes && i + 1 < line.size() && line[i + 1] == '"') {
                token += '"';
                ++i; // skip escaped quote
            } else {
                inQuotes = !inQuotes;
            }
        } else if (c == ',' && !inQuotes) {
            tokens.push_back(token);
            token.clear();
        } else {
            token += c;
        }
    }
    tokens.push_back(token);
    return tokens;
}

int TaskManager::addTask(const std::string& title,
                         const std::string& priority,
                         const std::string& category,
                         const std::string& dueDate,
                         bool isCompleted) {
    Task task;
    task.id = nextId++;
    task.title = title;
    task.isCompleted = isCompleted;
    task.priority = priority;
    task.category = category;
    task.dueDate = dueDate;

    tasks[task.id] = task;
    return task.id;
}

bool TaskManager::addTaskWithId(const Task& task) {
    tasks[task.id] = task;
    if (task.id >= nextId) {
        nextId = task.id + 1;
    }
    return true;
}

bool TaskManager::getTask(int id, Task& task) const {
    auto it = tasks.find(id);
    if (it != tasks.end()) {
        task = it->second;
        return true;
    }
    return false;
}

std::optional<Task> TaskManager::getTask(int id) const {
    auto it = tasks.find(id);
    if (it != tasks.end()) {
        return it->second;
    }
    return std::nullopt;
}

const std::map<int, Task>& TaskManager::getAllTasks() const {
    return tasks;
}

bool TaskManager::updateTask(const Task& task) {
    auto it = tasks.find(task.id);
    if (it != tasks.end()) {
        it->second = task;
        return true;
    }
    return false;
}

bool TaskManager::deleteTask(int id) {
    return tasks.erase(id) > 0;
}

bool TaskManager::completeTask(int id) {
    auto it = tasks.find(id);
    if (it != tasks.end()) {
        it->second.isCompleted = true;
        return true;
    }
    return false;
}

void TaskManager::clear() {
    tasks.clear();
    nextId = 1;
}

int TaskManager::getMaxId() const {
    if (tasks.empty()) {
        return 0;
    }
    return tasks.rbegin()->first;
}

bool TaskManager::saveToFile(const std::string& filename) const {
    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
        return false;
    }

    // Write CSV Header
    outFile << "id,title,isCompleted,priority,category,dueDate\n";

    for (const auto& [id, task] : tasks) {
        outFile << task.id << ","
                << escapeCsv(task.title) << ","
                << (task.isCompleted ? "1" : "0") << ","
                << escapeCsv(task.priority) << ","
                << escapeCsv(task.category) << ","
                << escapeCsv(task.dueDate) << "\n";
    }

    return outFile.good();
}

bool TaskManager::loadFromFile(const std::string& filename) {
    if (!std::filesystem::exists(filename)) {
        clear();
        return true;
    }

    std::ifstream inFile(filename);
    if (!inFile.is_open()) {
        clear();
        return true;
    }

    clear();
    int maxId = 0;
    std::string line;

    while (std::getline(inFile, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        if (line.empty()) {
            continue;
        }

        std::vector<std::string> tokens = parseCsvLine(line);
        if (tokens.empty()) {
            continue;
        }

        // Check if header line
        if (tokens[0] == "id") {
            continue;
        }

        if (tokens.size() >= 6) {
            try {
                Task task;
                task.id = std::stoi(tokens[0]);
                task.title = tokens[1];
                task.isCompleted = (tokens[2] == "1" || tokens[2] == "true" || tokens[2] == "TRUE" || tokens[2] == "True");
                task.priority = tokens[3];
                task.category = tokens[4];
                task.dueDate = tokens[5];

                tasks[task.id] = task;
                if (task.id > maxId) {
                    maxId = task.id;
                }
            } catch (...) {
                // Ignore malformed lines gracefully
                continue;
            }
        }
    }

    nextId = maxId + 1;
    return true;
}

} // namespace tasks
} // namespace assistant
