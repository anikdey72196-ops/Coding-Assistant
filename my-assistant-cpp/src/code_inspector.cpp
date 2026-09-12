#include "code_inspector.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <stack>
#include <sstream>
#include <regex>
#include <algorithm>

namespace fs = std::filesystem;

namespace assistant {
namespace inspector {

static std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

InspectionResult CodeInspector::inspectPath(const std::string& path) {
    InspectionResult combinedResult;
    fs::path p(path);

    if (!fs::exists(p)) {
        std::cerr << "Error: Path does not exist: " << path << std::endl;
        return combinedResult;
    }

    if (fs::is_directory(p)) {
        for (const auto& entry : fs::recursive_directory_iterator(p)) {
            if (entry.is_regular_file()) {
                std::string ext = entry.path().extension().string();
                if (ext == ".cpp" || ext == ".h" || ext == ".hpp") {
                    InspectionResult res = inspectFile(entry.path().string());
                    if (res.hasAnomalies) combinedResult.hasAnomalies = true;
                    if (res.hasFixes) combinedResult.hasFixes = true;
                    combinedResult.logs.insert(combinedResult.logs.end(), res.logs.begin(), res.logs.end());
                }
            }
        }
    } else if (fs::is_regular_file(p)) {
        combinedResult = inspectFile(path);
    }

    return combinedResult;
}

InspectionResult CodeInspector::inspectFile(const std::string& filepath) {
    InspectionResult result;
    std::ifstream infile(filepath);
    if (!infile.is_open()) {
        std::cerr << "Error: Could not open file " << filepath << std::endl;
        return result;
    }

    std::vector<std::string> lines;
    std::string line;
    while (std::getline(infile, line)) {
        lines.push_back(line);
    }
    infile.close();

    // 1. Delimiter checks (reporting only)
    checkDelimiters(lines, filepath, result);

    // 2. Header guard check & auto-repair
    bool fixedHeader = checkAndFixHeaderGuard(lines, filepath, result);

    // 3. Includes check & auto-repair
    bool fixedIncludes = checkAndFixIncludes(lines, filepath, result);

    // 4. Semicolon check & auto-repair
    bool fixedSemicolons = checkAndFixSemicolons(lines, filepath, result);

    if (fixedHeader || fixedIncludes || fixedSemicolons) {
        result.hasFixes = true;
        if (createBackup(filepath)) {
            saveFile(filepath, lines);
        } else {
            std::cerr << "Error: Failed to create backup for " << filepath << ". Fixes not saved." << std::endl;
        }
    }

    return result;
}

void CodeInspector::checkDelimiters(const std::vector<std::string>& lines, const std::string& filepath, InspectionResult& result) {
    struct DelimInfo {
        char type;
        size_t lineNum;
        size_t colNum;
    };

    std::stack<DelimInfo> delimStack;
    bool inBlockComment = false;

    for (size_t i = 0; i < lines.size(); ++i) {
        const std::string& l = lines[i];
        bool inString = false;
        char stringChar = 0;

        for (size_t j = 0; j < l.size(); ++j) {
            char c = l[j];

            if (inBlockComment) {
                if (c == '*' && j + 1 < l.size() && l[j + 1] == '/') {
                    inBlockComment = false;
                    j++;
                }
                continue;
            }

            if (!inString && c == '/' && j + 1 < l.size() && l[j + 1] == '*') {
                inBlockComment = true;
                j++;
                continue;
            }

            if (!inString && c == '/' && j + 1 < l.size() && l[j + 1] == '/') {
                break; // Line comment, ignore rest of line
            }

            if (c == '"' || c == '\'') {
                if (!inString) {
                    inString = true;
                    stringChar = c;
                } else if (stringChar == c && (j == 0 || l[j - 1] != '\\')) {
                    inString = false;
                }
                continue;
            }

            if (inString) continue;

            if (c == '{' || c == '(' || c == '[') {
                delimStack.push({c, i + 1, j + 1});
            } else if (c == '}' || c == ')' || c == ']') {
                if (delimStack.empty()) {
                    result.hasAnomalies = true;
                    std::string msg = "[ANOMALY] Line " + std::to_string(i + 1) + " in " + filepath + ": Unmatched closing delimiter '" + c + "'";
                    result.logs.push_back(msg);
                    std::cout << msg << std::endl;
                } else {
                    char topC = delimStack.top().type;
                    if ((c == '}' && topC == '{') ||
                        (c == ')' && topC == '(') ||
                        (c == ']' && topC == '[')) {
                        delimStack.pop();
                    } else {
                        result.hasAnomalies = true;
                        std::string msg = "[ANOMALY] Line " + std::to_string(i + 1) + " in " + filepath + ": Mismatched delimiter '" + c + "' for '" + topC + "' on line " + std::to_string(delimStack.top().lineNum);
                        result.logs.push_back(msg);
                        std::cout << msg << std::endl;
                        delimStack.pop();
                    }
                }
            }
        }
    }

    while (!delimStack.empty()) {
        result.hasAnomalies = true;
        DelimInfo top = delimStack.top();
        std::string msg = "[ANOMALY] Line " + std::to_string(top.lineNum) + " in " + filepath + ": Unclosed delimiter '" + top.type + "'";
        result.logs.push_back(msg);
        std::cout << msg << std::endl;
        delimStack.pop();
    }
}

bool CodeInspector::checkAndFixHeaderGuard(std::vector<std::string>& lines, const std::string& filepath, InspectionResult& result) {
    fs::path p(filepath);
    std::string ext = p.extension().string();
    if (ext != ".h" && ext != ".hpp") {
        return false;
    }

    bool hasGuard = false;
    for (const auto& line : lines) {
        std::string trimmed = trim(line);
        if (trimmed.rfind("#pragma once", 0) == 0 ||
            trimmed.rfind("#ifndef", 0) == 0 ||
            trimmed.rfind("#define", 0) == 0) {
            hasGuard = true;
            break;
        }
    }

    if (!hasGuard) {
        lines.insert(lines.begin(), "#pragma once");
        std::string msg = "[FIXED] Line 1 in " + filepath + ": Added missing header guard (#pragma once)";
        result.logs.push_back(msg);
        std::cout << msg << std::endl;
        return true;
    }

    return false;
}

bool CodeInspector::checkAndFixIncludes(std::vector<std::string>& lines, const std::string& filepath, InspectionResult& result) {
    bool usesString = false;
    bool usesVector = false;
    bool hasStringInclude = false;
    bool hasVectorInclude = false;

    // Scan lines
    for (const auto& line : lines) {
        std::string trimmed = trim(line);
        if (trimmed.find("#include <string>") != std::string::npos || trimmed.find("#include <string.h>") != std::string::npos) {
            hasStringInclude = true;
        }
        if (trimmed.find("#include <vector>") != std::string::npos) {
            hasVectorInclude = true;
        }

        // Simple check for std::string or std::vector usage (not in comments or includes)
        if (trimmed.rfind("#include", 0) != 0 && trimmed.rfind("//", 0) != 0) {
            if (trimmed.find("std::string") != std::string::npos || trimmed.find("string ") != std::string::npos) {
                usesString = true;
            }
            if (trimmed.find("std::vector") != std::string::npos || trimmed.find("vector<") != std::string::npos) {
                usesVector = true;
            }
        }
    }

    bool modified = false;
    size_t insertPos = 0;
    // Find proper insert position (after #pragma once if present, or top)
    if (!lines.empty() && trim(lines[0]) == "#pragma once") {
        insertPos = 1;
    }

    if (usesString && !hasStringInclude) {
        lines.insert(lines.begin() + insertPos, "#include <string>");
        insertPos++;
        modified = true;
        std::string msg = "[FIXED] Line " + std::to_string(insertPos) + " in " + filepath + ": Added missing include <string>";
        result.logs.push_back(msg);
        std::cout << msg << std::endl;
    }

    if (usesVector && !hasVectorInclude) {
        lines.insert(lines.begin() + insertPos, "#include <vector>");
        insertPos++;
        modified = true;
        std::string msg = "[FIXED] Line " + std::to_string(insertPos) + " in " + filepath + ": Added missing include <vector>";
        result.logs.push_back(msg);
        std::cout << msg << std::endl;
    }

    return modified;
}

bool CodeInspector::checkAndFixSemicolons(std::vector<std::string>& lines, const std::string& filepath, InspectionResult& result) {
    bool modified = false;
    bool inStructOrClass = false;

    for (size_t i = 0; i < lines.size(); ++i) {
        std::string originalLine = lines[i];
        std::string trimmed = trim(originalLine);

        if (trimmed.empty()) continue;

        // Skip preprocessor directives, comments
        if (trimmed[0] == '#' || trimmed.rfind("//", 0) == 0 || trimmed.rfind("/*", 0) == 0) {
            continue;
        }

        // Track struct / class declaration start
        if (trimmed.rfind("struct ", 0) == 0 || trimmed.rfind("class ", 0) == 0) {
            inStructOrClass = true;
        }

        // Strip inline comments for checking end of statement
        std::string codePart = trimmed;
        std::string commentPart = "";
        size_t commentPos = trimmed.find("//");
        if (commentPos != std::string::npos) {
            codePart = trim(trimmed.substr(0, commentPos));
            commentPart = " " + trimmed.substr(commentPos);
        }

        if (codePart.empty()) continue;

        char lastChar = codePart.back();

        // Check if line should end with a semicolon
        bool needsSemicolon = false;

        if (lastChar == '}') {
            if (inStructOrClass) {
                needsSemicolon = true;
                inStructOrClass = false; // Closed struct/class
            }
        } else if (lastChar != ';' && lastChar != '{' && lastChar != '}' && lastChar != ':' && lastChar != ',' && lastChar != '\\') {
            // Check if it's NOT a control flow construct or function header
            bool isControlFlow = (codePart.rfind("if", 0) == 0 ||
                                 codePart.rfind("if ", 0) == 0 ||
                                 codePart.rfind("if(", 0) == 0 ||
                                 codePart.rfind("for", 0) == 0 ||
                                 codePart.rfind("while", 0) == 0 ||
                                 codePart.rfind("else", 0) == 0 ||
                                 codePart.rfind("namespace", 0) == 0 ||
                                 codePart.rfind("case ", 0) == 0 ||
                                 codePart.rfind("default:", 0) == 0 ||
                                 codePart.rfind("switch", 0) == 0 ||
                                 codePart.rfind("try", 0) == 0 ||
                                 codePart.rfind("catch", 0) == 0);

            if (!isControlFlow) {
                // If it ends in ')' check if next non-empty line starts with '{' (function definition)
                if (lastChar == ')') {
                    bool followedByOpenBrace = false;
                    for (size_t k = i + 1; k < lines.size(); ++k) {
                        std::string nextTrimmed = trim(lines[k]);
                        if (!nextTrimmed.empty()) {
                            if (nextTrimmed[0] == '{') {
                                followedByOpenBrace = true;
                            }
                            break;
                        }
                    }
                    if (!followedByOpenBrace) {
                        needsSemicolon = true;
                    }
                } else {
                    needsSemicolon = true;
                }
            }
        }

        if (needsSemicolon) {
            // Reconstruct line with semicolon before commentPart
            size_t codeInOriginal = originalLine.find(codePart);
            if (codeInOriginal != std::string::npos) {
                size_t insertIndex = codeInOriginal + codePart.length();
                std::string newLine = originalLine.substr(0, insertIndex) + ";" + originalLine.substr(insertIndex);
                lines[i] = newLine;
                modified = true;

                std::string msg = "[FIXED] Line " + std::to_string(i + 1) + " in " + filepath + ": Added missing semicolon";
                result.logs.push_back(msg);
                std::cout << msg << std::endl;
            }
        }
    }

    return modified;
}

bool CodeInspector::createBackup(const std::string& filepath) {
    std::string backupPath = filepath + ".bak";
    try {
        fs::copy_file(filepath, backupPath, fs::copy_options::overwrite_existing);
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Backup creation error: " << e.what() << std::endl;
        return false;
    }
}

bool CodeInspector::saveFile(const std::string& filepath, const std::vector<std::string>& lines) {
    std::ofstream outfile(filepath, std::ios::trunc);
    if (!outfile.is_open()) return false;

    for (size_t i = 0; i < lines.size(); ++i) {
        outfile << lines[i] << "\n";
    }
    outfile.close();
    return true;
}

} // namespace inspector
} // namespace assistant
