if(HAVE_GTEST)
    # Comprehensive test suite with GTest
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
    target_link_libraries(run_tests PRIVATE 
        distcache_core 
        GTest::GTest 
        GTest::Main
    )
    
    # Enable CTest
    enable_testing()
    add_test(NAME DistCacheTests COMMAND run_tests)
    
    message(STATUS "Building comprehensive test suite with GTest")
else()
    # Minimal test executable without GTest
    add_executable(run_tests minimal_test.cpp)
    target_link_libraries(run_tests PRIVATE distcache_core)
    
    message(STATUS "Building minimal test suite without GTest")
endif()

# Set test properties
set_target_properties(run_tests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Install tests
install(TARGETS run_tests DESTINATION bin)