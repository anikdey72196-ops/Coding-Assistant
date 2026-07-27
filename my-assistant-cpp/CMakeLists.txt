cmake_minimum_required(VERSION 3.20)
project(my-assistant VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Enable common warnings
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()
# --- Download External Libraries ---
include(FetchContent)

# 1. Download JSON parser (For Gemma)
FetchContent_Declare(json
    URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
)
FetchContent_MakeAvailable(json)

# 2. Download HTTP Client (For Gemma)
FetchContent_Declare(httplib
    URL https://github.com/yhirose/cpp-httplib/archive/refs/tags/v0.16.0.tar.gz
)
FetchContent_MakeAvailable(httplib)

# 3. Download SQLiteCpp (For the Database)
FetchContent_Declare(SQLiteCpp
    GIT_REPOSITORY https://github.com/SRombauts/SQLiteCpp.git
    GIT_TAG        3.3.1
)
set(SQLITECPP_RUN_CPPLINT OFF CACHE BOOL "Disable linting" FORCE)
FetchContent_MakeAvailable(SQLiteCpp)

# Collect all source files
file(GLOB_RECURSE SRC_FILES "src/*.cpp")
list(FILTER SRC_FILES EXCLUDE REGEX "src/main.cpp")

# Create a library from the source files to link with main and tests
add_library(assistant_lib ${SRC_FILES})
target_include_directories(assistant_lib PUBLIC include src)
target_link_libraries(assistant_lib PUBLIC nlohmann_json::nlohmann_json httplib::httplib SQLiteCpp)
# Main executable
add_executable(my-assistant src/main.cpp)
target_link_libraries(my-assistant PRIVATE assistant_lib)

# Tests
enable_testing()
add_subdirectory(tests)
