#============================================================================
# benchmarks/CMakeLists.txt - Benchmark Configuration
#============================================================================
# Benchmark executable
add_executable(run_benchmark benchmark.cpp)
target_link_libraries(run_benchmark PRIVATE distcache_core)

# Set benchmark properties
set_target_properties(run_benchmark PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Install benchmark
install(TARGETS run_benchmark DESTINATION bin)