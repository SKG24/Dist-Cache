cmake_minimum_required(VERSION 3.16)
project(DistCache)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compiler flags for optimization and debugging
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Wextra -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

# Find required packages
find_package(Threads REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(MSGPACK REQUIRED msgpack)

# Include directories
include_directories(src)
include_directories(${MSGPACK_INCLUDE_DIRS})

# Core library sources
set(CORE_SOURCES
    src/storage/LRUCache.cpp
    src/storage/WAL.cpp
    src/storage/MMapPersistence.cpp
    src/network/RESPParser.cpp
    src/network/TCPServer.cpp
    src/cluster/HashRing.cpp
    src/cluster/NodeDiscovery.cpp
    src/patterns/CircuitBreaker.cpp
    src/monitoring/MetricsCollector.cpp
    src/monitoring/HttpDashboard.cpp
)

# Create core library
add_library(distcache_core ${CORE_SOURCES})
target_link_libraries(distcache_core 
    Threads::Threads 
    ${MSGPACK_LIBRARIES}
)

# Main executable
add_executable(distcache main.cpp)
target_link_libraries(distcache distcache_core)

# Add subdirectories
add_subdirectory(benchmarks)
add_subdirectory(tests)

# Install targets
install(TARGETS distcache DESTINATION bin)
