#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>
#include "code_inspector.h"

namespace fs = std::filesystem;
using namespace assistant::inspector;

class CodeInspectorTest : public ::testing::Test {
protected:
    std::string testDir = "test_sandbox";

    void SetUp() override {
        if (fs::exists(testDir)) {
            fs::remove_all(testDir);
        }
        fs::create_directory(testDir);
    }

    void TearDown() override {
        if (fs::exists(testDir)) {
            fs::remove_all(testDir);
        }
    }

    void createFile(const std::string& filename, const std::string& content) {
        std::ofstream outfile(filename);
        outfile << content;
        outfile.close();
    }

    std::string readFile(const std::string& filename) {
        std::ifstream infile(filename);
        std::stringstream buffer;
        buffer << infile.rdbuf();
        return buffer.str();
    }
};

TEST_F(CodeInspectorTest, DelimiterCheckAnomalies) {
    std::string filepath = testDir + "/delim_test.cpp";
    createFile(filepath, "void main() { int x = (5 + 3;\n");

    CodeInspector inspector;
    InspectionResult res = inspector.inspectFile(filepath);

    EXPECT_TRUE(res.hasAnomalies);
}

TEST_F(CodeInspectorTest, MissingSemicolonAndBackup) {
    std::string filepath = testDir + "/semicolon_test.cpp";
    createFile(filepath, "int main() {\n    int x = 5\n    return 0\n}\n");

    CodeInspector inspector;
    InspectionResult res = inspector.inspectFile(filepath);

    EXPECT_TRUE(res.hasFixes);
    EXPECT_TRUE(fs::exists(filepath + ".bak"));

    std::string fixedContent = readFile(filepath);
    EXPECT_NE(fixedContent.find("int x = 5;"), std::string::npos);
    EXPECT_NE(fixedContent.find("return 0;"), std::string::npos);
}

TEST_F(CodeInspectorTest, MissingIncludesFix) {
    std::string filepath = testDir + "/includes_test.cpp";
    createFile(filepath, "int main() {\n    std::string name = \"Jules\";\n    std::vector<int> nums;\n    return 0;\n}\n");

    CodeInspector inspector;
    InspectionResult res = inspector.inspectFile(filepath);

    EXPECT_TRUE(res.hasFixes);

    std::string fixedContent = readFile(filepath);
    EXPECT_NE(fixedContent.find("#include <string>"), std::string::npos);
    EXPECT_NE(fixedContent.find("#include <vector>"), std::string::npos);
}

TEST_F(CodeInspectorTest, MissingHeaderGuardFix) {
    std::string filepath = testDir + "/sample.hpp";
    createFile(filepath, "struct Point {\n    int x;\n    int y;\n};\n");

    CodeInspector inspector;
    InspectionResult res = inspector.inspectFile(filepath);

    EXPECT_TRUE(res.hasFixes);

    std::string fixedContent = readFile(filepath);
    EXPECT_NE(fixedContent.find("#pragma once"), std::string::npos);
}

TEST_F(CodeInspectorTest, DirectoryRecursionInspection) {
    std::string subDir = testDir + "/sub";
    fs::create_directory(subDir);

    std::string file1 = testDir + "/file1.cpp";
    std::string file2 = subDir + "/file2.h";

    createFile(file1, "int main() {\n    int a = 10\n}\n");
    createFile(file2, "struct Dummy {\n    int val;\n}\n");

    CodeInspector inspector;
    InspectionResult res = inspector.inspectPath(testDir);

    EXPECT_TRUE(res.hasFixes);
    EXPECT_TRUE(fs::exists(file1 + ".bak"));
    EXPECT_TRUE(fs::exists(file2 + ".bak"));
}
