cmake_minimum_required(VERSION 3.16)
project(DistCache VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler flags
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Wextra -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -Wall")

# Default to Release build
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Find required packages
find_package(Threads REQUIRED)

# Optional packages
set(HAVE_MSGPACK FALSE)
set(HAVE_GTEST FALSE)

# Try to find msgpack (optional)
find_path(MSGPACK_INCLUDE_DIR msgpack.hpp 
    PATHS 
    /usr/include
    /usr/local/include
    /opt/homebrew/include
)

if(MSGPACK_INCLUDE_DIR)
    message(STATUS "Found msgpack at: ${MSGPACK_INCLUDE_DIR}")
    set(HAVE_MSGPACK TRUE)
    add_definitions(-DHAVE_MSGPACK)
else()
    message(STATUS "msgpack not found - using text-only WAL")
endif()

# Try to find GTest (optional)
find_package(GTest QUIET)
if(GTest_FOUND)
    message(STATUS "Found GTest - building comprehensive tests")
    set(HAVE_GTEST TRUE)
else()
    message(STATUS "GTest not found - building minimal tests")
endif()

# Include directories
include_directories(src)
if(MSGPACK_INCLUDE_DIR)
    include_directories(${MSGPACK_INCLUDE_DIR})
endif()

# Add subdirectories
add_subdirectory(src)
add_subdirectory(benchmarks)
add_subdirectory(tests)

# Print configuration summary
message(STATUS "")
message(STATUS "=== DistCache Build Configuration ===")
message(STATUS "Build Type:      ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard:    ${CMAKE_CXX_STANDARD}")
message(STATUS "msgpack Support: ${HAVE_MSGPACK}")
message(STATUS "GTest Support:   ${HAVE_GTEST}")
message(STATUS "Install Prefix:  ${CMAKE_INSTALL_PREFIX}")
message(STATUS "====================================")
message(STATUS "")