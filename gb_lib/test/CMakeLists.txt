cmake_minimum_required(VERSION 3.11)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        15460959cbbfa20e66ef0b5ab497367e47fc0a04 # v1.12.0
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

set(SOURCES
  src/blargg_test.cpp
  src/cartridge_test.cpp
  src/resource_helper.h
  src/resource_helper.cpp
)

add_executable(gb_lib_test ${SOURCES})
target_link_libraries(gb_lib_test gb_lib gtest_main)

string(CONCAT RESOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/resources)
target_compile_definitions(gb_lib_test PRIVATE RESOURCE_PATH=\"${RESOURCE_PATH}\")

include(GoogleTest)
gtest_discover_tests(gb_lib_test)