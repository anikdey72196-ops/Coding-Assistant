#include <gtest/gtest.h>
#include "task_manager.h"
#include <filesystem>

class TaskManagerTest : public ::testing::Test {
protected:
    const std::string testFilename = "test_tasks_temp.csv";

    void TearDown() override {
        if (std::filesystem::exists(testFilename)) {
            std::filesystem::remove(testFilename);
        }
    }
};

TEST_F(TaskManagerTest, SaveAndLoadTwoTasks) {
    TaskManager tm1;
    int id1 = tm1.addTask("Write unit tests", "high", "development", "2026-10-15", false);
    int id2 = tm1.addTask("Review pull request", "medium", "code-review", "2026-10-16", true);

    EXPECT_EQ(id1, 1);
    EXPECT_EQ(id2, 2);

    EXPECT_TRUE(tm1.saveToFile(testFilename));

    TaskManager tm2;
    EXPECT_TRUE(tm2.loadFromFile(testFilename));

    const auto& tasks = tm2.getAllTasks();
    EXPECT_EQ(tasks.size(), 2u);

    auto t1Opt = tm2.getTask(1);
    ASSERT_TRUE(t1Opt.has_value());
    EXPECT_EQ(t1Opt->id, 1);
    EXPECT_EQ(t1Opt->title, "Write unit tests");
    EXPECT_FALSE(t1Opt->isCompleted);
    EXPECT_EQ(t1Opt->priority, "high");
    EXPECT_EQ(t1Opt->category, "development");
    EXPECT_EQ(t1Opt->dueDate, "2026-10-15");

    auto t2Opt = tm2.getTask(2);
    ASSERT_TRUE(t2Opt.has_value());
    EXPECT_EQ(t2Opt->id, 2);
    EXPECT_EQ(t2Opt->title, "Review pull request");
    EXPECT_TRUE(t2Opt->isCompleted);
    EXPECT_EQ(t2Opt->priority, "medium");
    EXPECT_EQ(t2Opt->category, "code-review");
    EXPECT_EQ(t2Opt->dueDate, "2026-10-16");
}

TEST_F(TaskManagerTest, TrackMaxIdAndContinueFromMaxIdPlusOne) {
    TaskManager tm1;
    tm1.addTask("Task 1", "low", "general", "2026-10-01");
    tm1.addTask("Task 2", "high", "general", "2026-10-02");

    EXPECT_TRUE(tm1.saveToFile(testFilename));

    TaskManager tm2;
    EXPECT_TRUE(tm2.loadFromFile(testFilename));

    EXPECT_EQ(tm2.getMaxId(), 2);
    EXPECT_EQ(tm2.getNextId(), 3);

    int newId = tm2.addTask("Task 3", "medium", "work", "2026-10-03");
    EXPECT_EQ(newId, 3);
    EXPECT_EQ(tm2.getAllTasks().size(), 3u);
}

TEST_F(TaskManagerTest, NonExistentFileStartsFreshWithoutCrashing) {
    TaskManager tm;
    const std::string nonExistentFile = "definitely_non_existent_file_12345.csv";
    if (std::filesystem::exists(nonExistentFile)) {
        std::filesystem::remove(nonExistentFile);
    }

    EXPECT_TRUE(tm.loadFromFile(nonExistentFile));
    EXPECT_TRUE(tm.getAllTasks().empty());
    EXPECT_EQ(tm.getNextId(), 1);
}

TEST_F(TaskManagerTest, HandleSpecialCharactersInCSV) {
    TaskManager tm1;
    tm1.addTask("Task with comma, and \"quotes\"", "urgent,high", "work,office", "2026-12-31", false);

    EXPECT_TRUE(tm1.saveToFile(testFilename));

    TaskManager tm2;
    EXPECT_TRUE(tm2.loadFromFile(testFilename));

    auto t1 = tm2.getTask(1);
    ASSERT_TRUE(t1.has_value());
    EXPECT_EQ(t1->title, "Task with comma, and \"quotes\"");
    EXPECT_EQ(t1->priority, "urgent,high");
    EXPECT_EQ(t1->category, "work,office");
}
