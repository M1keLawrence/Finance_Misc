cmake_minimum_required(VERSION 3.16)

project(BoostIPC LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find Boost.Interprocess
find_package(Boost REQUIRED)

# Executables for Problem 1
# Writer executable
add_executable(Problem1_writer writer.cpp)
target_include_directories(Problem1_writer PRIVATE ${Boost_INCLUDE_DIRS})

# Reader executable
add_executable(Problem1_reader reader.cpp)
target_include_directories(Problem1_reader PRIVATE ${Boost_INCLUDE_DIRS})

# Executable for Problem 2:
add_executable(Problem2 test_productsP2.cpp)
target_include_directories(Problem2 PRIVATE ${Boost_INCLUDE_DIRS})

# Executable for Problem 3:
add_executable(Problem3 test_productsP3.cpp)
target_include_directories(Problem3 PRIVATE ${Boost_INCLUDE_DIRS})
