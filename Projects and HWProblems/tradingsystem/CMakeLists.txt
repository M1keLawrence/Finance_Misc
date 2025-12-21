cmake_minimum_required(VERSION 3.16)
project(BondTradingSystem LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Boost via Homebrew is headers + (optional) libs. We only need headers.
# If you installed Boost via brew, its headers are usually already in the default include path,
# but we add a hint path anyway.
include_directories(/usr/local/include)
include_directories(/opt/homebrew/include) # Apple Silicon
include_directories(${CMAKE_SOURCE_DIR})

find_package(Threads REQUIRED)

add_definitions(-DBOOST_ASIO_DISABLE_CONCEPTS)
add_definitions(-DBOOST_ASIO_NO_DEPRECATED)

add_executable(exec_print ExecutionPrintProcess.cpp)
target_link_libraries(exec_print PRIVATE pthread)

add_executable(stream_print StreamingPrintProcess.cpp)
target_link_libraries(stream_print PRIVATE pthread)

add_executable(gen_data DataGenerators.cpp)
target_link_libraries(gen_data PRIVATE pthread)

add_executable(trading_system main.cpp)
target_link_libraries(trading_system PRIVATE pthread)

add_executable(md_shm_publisher md_shm_publisher.cpp)
target_link_libraries(md_shm_publisher PRIVATE Threads::Threads)

add_executable(prices_publisher prices_publisher_main.cpp)
add_executable(trades_publisher trades_publisher_main.cpp)
add_executable(inquiries_publisher inquiries_publisher_main.cpp)

foreach(t trading_system exec_print stream_print gen_data md_shm_publisher
          prices_publisher trades_publisher inquiries_publisher)
  target_include_directories(${t} PRIVATE
    ${CMAKE_SOURCE_DIR}
    /usr/local/include
    /opt/homebrew/include
  )
  target_link_libraries(${t} PRIVATE Threads::Threads)
endforeach()

