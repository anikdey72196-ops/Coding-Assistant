#pragma once

#include <string>
#include <vector>

namespace assistant {
namespace inspector {

struct InspectionResult {
    bool hasAnomalies = false;
    bool hasFixes = false;
    std::vector<std::string> logs;
};

class CodeInspector {
public:
    CodeInspector() = default;

    // Inspects a single file or recursively inspects a directory
    InspectionResult inspectPath(const std::string& path);

    // Inspects a single file (returns result for that file)
    InspectionResult inspectFile(const std::string& filepath);

private:
    void checkDelimiters(const std::vector<std::string>& lines, const std::string& filename, InspectionResult& result);
    bool checkAndFixHeaderGuard(std::vector<std::string>& lines, const std::string& filepath, InspectionResult& result);
    bool checkAndFixIncludes(std::vector<std::string>& lines, const std::string& filepath, InspectionResult& result);
    bool checkAndFixSemicolons(std::vector<std::string>& lines, const std::string& filepath, InspectionResult& result);

    bool createBackup(const std::string& filepath);
    bool saveFile(const std::string& filepath, const std::vector<std::string>& lines);
};

} // namespace inspector
} // namespace assistant
