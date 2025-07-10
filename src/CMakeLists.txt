# Core library sources
set(CORE_SOURCES
    storage/LRUCache.cpp
    storage/WAL.cpp
    storage/MMapPersistence.cpp
    network/RESPParser.cpp
    network/TCPServer.cpp
    cluster/HashRing.cpp
    cluster/NodeDiscovery.cpp
    patterns/CircuitBreaker.cpp
    monitoring/MetricsCollector.cpp
    monitoring/HttpDashboard.cpp
)

# Create core library
add_library(distcache_core STATIC ${CORE_SOURCES})
target_include_directories(distcache_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(distcache_core PUBLIC Threads::Threads)

# Set library properties
set_target_properties(distcache_core PROPERTIES
    POSITION_INDEPENDENT_CODE ON
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON
)

# Main executable
add_executable(distcache ${CMAKE_SOURCE_DIR}/main.cpp)
target_link_libraries(distcache PRIVATE distcache_core)

# Install targets
install(TARGETS distcache distcache_core
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

