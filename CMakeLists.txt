cmake_minimum_required(VERSION 3.10)
project(ThreadSafeQueue VERSION 1.0)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
add_executable(BoschTest qeuethread.cpp)

# cmake_minimum_required(VERSION 3.10)

# # Project name and version
# project(QueueTestProject VERSION 1.0)

# # Set the C++ standard
# set(CMAKE_CXX_STANDARD 11)
# set(CMAKE_CXX_STANDARD_REQUIRED True)

# # Include FetchContent module
# include(FetchContent)

# # Download and configure GoogleTest
# FetchContent_Declare(
#   googletest
#   URL https://github.com/google/googletest/archive/release-1.10.0.zip
# )
# # For Windows: Prevent overriding the parent project's compiler/linker settings
# set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
# FetchContent_MakeAvailable(googletest)

# # Add the test executable
# add_executable(test_queue test_queue.cpp)

# # Link the test executable against GoogleTest
# target_link_libraries(test_queue gtest gtest_main)

# # Include GoogleTest directories
# target_include_directories(test_queue PRIVATE ${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

# # Enable testing
# enable_testing()

# # Add tests
# add_test(NAME test_queue COMMAND test_queue)