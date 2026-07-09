include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Collect test files
file(GLOB_RECURSE TEST_FILES "test_*.cpp")

add_executable(assistant_tests ${TEST_FILES})
target_link_libraries(assistant_tests
  PRIVATE
  assistant_lib
  gtest_main
)

include(GoogleTest)
gtest_discover_tests(assistant_tests)
