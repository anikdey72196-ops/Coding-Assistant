#pragma once

#include <string>
#include <map>
#include <vector>
#include <optional>

namespace assistant {
namespace tasks {

struct Task {
    int id{0};
    std::string title;
    bool isCompleted{false};
    std::string priority;
    std::string category;
    std::string dueDate;

    bool operator==(const Task& other) const {
        return id == other.id &&
               title == other.title &&
               isCompleted == other.isCompleted &&
               priority == other.priority &&
               category == other.category &&
               dueDate == other.dueDate;
    }
};

class TaskManager {
private:
    std::map<int, Task> tasks;
    int nextId{1};

public:
    TaskManager() = default;

    int addTask(const std::string& title,
               const std::string& priority = "",
               const std::string& category = "",
               const std::string& dueDate = "",
               bool isCompleted = false);

    bool addTaskWithId(const Task& task);

    bool getTask(int id, Task& task) const;
    std::optional<Task> getTask(int id) const;
    const std::map<int, Task>& getAllTasks() const;
    const std::map<int, Task>& getTasks() const { return getAllTasks(); }

    bool updateTask(const Task& task);
    bool deleteTask(int id);
    bool completeTask(int id);
    void clear();

    int getNextId() const { return nextId; }
    int getMaxId() const;

    bool saveToFile(const std::string& filename = "tasks.csv") const;
    bool loadFromFile(const std::string& filename = "tasks.csv");
};

} // namespace tasks

using Task = tasks::Task;
using TaskManager = tasks::TaskManager;

} // namespace assistant

using Task = assistant::tasks::Task;
using TaskManager = assistant::tasks::TaskManager;
