add_executable(run_benchmark
    benchmark.cpp
)

target_include_directories(run_benchmark PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(run_benchmark distcache_core)