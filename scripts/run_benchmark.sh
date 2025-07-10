#!/bin/bash

echo "=== DistCache Benchmark Suite ==="

# Build if needed
if [ ! -f "build/benchmark/run_benchmark" ]; then
    echo "Building project..."
    mkdir -p build
    cd build
    cmake ..
    make -j$(nproc)
    cd ..
fi

# Run benchmarks
echo "Running comprehensive benchmarks..."
build/benchmark/run_benchmark | tee benchmark_results.txt

# Generate performance report
echo ""
echo "=== Performance Summary ==="
grep "ops/sec" benchmark_results.txt | head -10

echo ""
echo "Results saved to benchmark_results.txt"

# Optional: Compare with previous results
if [ -f "previous_benchmark_results.txt" ]; then
    echo ""
    echo "=== Performance Comparison ==="
    echo "Current vs Previous:"
    diff -u previous_benchmark_results.txt benchmark_results.txt || true
fi

# Save current results as previous for next run
cp benchmark_results.txt previous_benchmark_results.txt

echo "Benchmark suite completed successfully!"