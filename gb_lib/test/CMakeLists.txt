cmake_minimum_required(VERSION 3.11)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        703bd9caab50b139428cea1aaff9974ebee5742e # release-1.10.0
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

set(SOURCES
    src/cpu_test.cpp
)

add_executable(gb_lib_test ${SOURCES})
target_link_libraries(gb_lib_test gtest_main)

include(GoogleTest)
gtest_discover_tests(gb_lib_test)