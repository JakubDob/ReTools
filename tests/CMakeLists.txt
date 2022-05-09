cmake_minimum_required(VERSION 3.22.1)

project(REUtilsTests)

add_executable(
    REtests
    tests.cpp
)
target_compile_features(REtests PRIVATE cxx_std_20)
# target_link_options(REtests PRIVATE "/DYNAMICBASE:NO") #disable aslr
enable_testing()

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_dependencies(
    REtests
    REutils
)

target_link_libraries(
    REtests
    PRIVATE
    REutils
    gtest_main
)

target_include_directories(
    REtests
    PRIVATE
    $<TARGET_PROPERTY:REutils,INCLUDE_DIRECTORIES>
)

include(GoogleTest)
gtest_discover_tests(
    REtests
    DISCOVERY_MODE PRE_TEST
)