find_package(GTest REQUIRED)

set(TEST_SOURCES
    test_main.cpp
    test_LRUCache.cpp
    test_HashRing.cpp
    test_CircuitBreaker.cpp
    test_MMapPersistence.cpp
    test_MetricsCollector.cpp
    test_RESPParser.cpp
    test_WAL.cpp
)

add_executable(run_tests ${TEST_SOURCES})

target_include_directories(run_tests PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(run_tests 
    distcache_core
    GTest::GTest 
    GTest::Main
)

# Add tests to CTest
enable_testing()
add_test(NAME DistCacheTests COMMAND run_tests)