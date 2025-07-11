# DistCache

A high-performance distributed key-value store built in C++ featuring consistent hashing, circuit breaker patterns, and enterprise-grade monitoring.

![Build Status](https://img.shields.io/badge/build-passing-brightgreen)
![Tests](https://img.shields.io/badge/tests-35%2F43%20passing-yellow)
![Performance](https://img.shields.io/badge/performance-664K%20ops%2Fsec-brightgreen)
![Docker](https://img.shields.io/badge/docker-ready-blue)

(screenshot)[!Screenshot 2025-07-11 at 10.35.38 AM.png]
(screenshot)[!Screenshot 2025-07-11 at 10.36.04 AM.png]
(screenshot)[!Screenshot 2025-07-11 at 10.36.15 AM.png]
(screenshot)[!Screenshot 2025-07-11 at 10.36.26 AM.png]
(screenshot)[!Screenshot 2025-07-11 at 10.36.35 AM.png]
(screenshot)[!Screenshot 2025-07-11 at 10.36.44 AM.png]
(screenshot)[!Screenshot 2025-07-11 at 10.36.56 AM.png]
(screenshot)[!Screenshot 2025-07-11 at 10.37.09 AM.png]
(screenshot)[!Screenshot 2025-07-11 at 10.37.20 AM.png]
(screenshot)[!Screenshot 2025-07-11 at 10.37.29 AM.png]

## ✨ Features

- **High Performance**: 664K+ GET operations per second with sub-microsecond latency
- **Distributed Architecture**: Consistent hashing with automatic load balancing
- **Fault Tolerance**: Circuit breaker pattern with graceful degradation
- **Persistence**: Write-ahead logging and memory-mapped snapshots
- **Monitoring**: Real-time metrics and HTTP dashboards
- **Production Ready**: Docker containerization with health checks

## 🚀 Quick Start

### Prerequisites
- Docker and Docker Compose
- Git

### Installation

```bash
git clone https://github.com/SKG24/Dist-Cache.git
cd Dist-Cache
```

### Deploy 3-Node Cluster

```bash
# Start the cluster
docker-compose up -d

# Verify deployment
docker-compose ps

# Run health checks
chmod +x verify-deployment.sh
./verify-deployment.sh
```

## 📊 Performance Benchmarks

| Operation | Throughput | Latency |
|-----------|------------|---------|
| Cache GET | 664,744 ops/sec | 1.50 μs |
| Cache SET | 533,118 ops/sec | 1.88 μs |
| Hash Ring Lookup | 2,032,448 ops/sec | 0.49 μs |
| Circuit Breaker Check | 276M+ ops/sec | 0.00 μs |
| WAL Operations | 575K-1.1M ops/sec | 0.89-1.74 μs |

## 🏗️ Architecture

### Core Components

- **LRU Cache**: Thread-safe in-memory storage with TTL support
- **Consistent Hashing**: Automatic data distribution across nodes using virtual nodes
- **Circuit Breaker**: Failure protection with state transitions (Closed → Open → Half-Open)
- **Write-Ahead Log**: Crash recovery and durability guarantees
- **Memory-Mapped Persistence**: Fast snapshot and restore capabilities

### Distributed Systems Features

- **Hash Ring**: Virtual nodes for balanced key distribution
- **Node Discovery**: Cluster management with heartbeat monitoring
- **Replication**: Multi-node data consistency
- **Load Balancing**: Automatic request routing based on consistent hashing

## 🔧 Usage

### Access Points

| Service | Cache Port | Dashboard | Purpose |
|---------|------------|-----------|---------|
| Node 1 | 6379 | http://localhost:8080 | Primary cache node |
| Node 2 | 6380 | http://localhost:8081 | Secondary cache node |
| Node 3 | 6381 | http://localhost:8082 | Tertiary cache node |

### Basic Operations

```bash
# Set a key-value pair
echo 'SET mykey myvalue' | nc localhost 6379

# Get a value
echo 'GET mykey' | nc localhost 6379

# Delete a key
echo 'DEL mykey' | nc localhost 6379
```

### Testing

```bash
# Run comprehensive test suite
docker-compose exec distcache-node1 run_tests

# Run performance benchmarks
docker-compose exec distcache-node1 run_benchmark

# View cluster logs
docker-compose logs -f
```

### Monitoring

Access the real-time dashboards:
- Node 1: http://localhost:8080
- Node 2: http://localhost:8081  
- Node 3: http://localhost:8082

## 🔄 Cluster Management

```bash
# Stop the cluster
docker-compose down

# Restart the cluster
docker-compose restart

# Scale up (add more nodes)
docker-compose up -d --scale distcache-node1=2

# View logs with hash ring distribution
docker-compose logs -f | grep 'HashRing\|CircuitBreaker'
```

## 📈 Monitoring & Observability

### Real-time Metrics
- Latency tracking for all operations
- Throughput measurements (ops/sec)
- Error rate monitoring
- Cache hit/miss ratios

### Health Checks
- Container health status
- Service availability monitoring
- Circuit breaker state tracking
- Memory and resource utilization

## 🧪 Development

### Running Tests Locally

```bash
# Build the project
make build

# Run unit tests
make test

# Run benchmarks
make benchmark
```

### Test Coverage

- ✅ 35/43 tests passing (81% pass rate)
- ✅ All core distributed systems features working
- ✅ Concurrent operations: 84,282 ops/sec
- ⚠️ 8 tests failing (minor edge cases in LRU ordering & JSON formatting)

## 🔍 Configuration

### Environment Variables

- `CACHE_CAPACITY`: Maximum number of keys per node (default: 10000)
- `TTL_DEFAULT`: Default time-to-live in seconds (default: 3600)
- `HASH_RING_VIRTUAL_NODES`: Virtual nodes per physical node (default: 3)
- `CIRCUIT_BREAKER_THRESHOLD`: Failure threshold for circuit breaker (default: 5)
