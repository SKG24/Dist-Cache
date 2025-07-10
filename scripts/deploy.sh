#!/bin/bash

set -e

echo "=== DistCache Deployment Script ==="

# Build and test
echo "Building project..."
docker-compose build

echo "Running tests..."
docker run --rm distcache_distcache-node1 run_tests

echo "Running benchmarks..."
docker run --rm distcache_distcache-node1 run_benchmark

# Deploy cluster
echo "Deploying 3-node cluster..."
docker-compose up -d

echo "Waiting for services to start..."
sleep 10

# Health checks
echo "Checking node health..."
for port in 6379 6380 6381; do
    if nc -z localhost $port; then
        echo "✓ Node on port $port is healthy"
    else
        echo "✗ Node on port $port is not responding"
    fi
done

echo "=== Deployment Complete ==="
echo "Services:"
echo "  - Node 1: localhost:6379 (Dashboard: localhost:8080)"
echo "  - Node 2: localhost:6380 (Dashboard: localhost:8081)" 
echo "  - Node 3: localhost:6381 (Dashboard: localhost:8082)"
echo ""
echo "To test: docker exec -it distcache-cli redis-cli -h distcache-node1"
echo "To view logs: docker-compose logs -f"
echo "To stop: docker-compose down"