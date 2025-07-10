# DistCache: Enhanced Distributed Key-Value Store

A production-ready, distributed cache implementation in C++ featuring consistent hashing, circuit breaker pattern, and comprehensive monitoring.

## 🚀 Features

### Core Storage
- **LRU Cache**: Thread-safe in-memory cache with TTL support
- **Write-Ahead Logging**: Crash recovery with binary/text formats
- **Memory-Mapped Persistence**: Fast snapshot/restore capabilities

### Distributed Systems
- **Consistent Hashing**: Automatic data partitioning with virtual nodes
- **Circuit Breaker**: Failure protection with configurable thresholds
- **Node Discovery**: Simulated cluster management and heartbeats

### Monitoring & Reliability
- **Real-time Metrics**: Latency tracking, throughput monitoring
- **HTTP Dashboard**: Live system status and statistics
- **Health Checks**: Comprehensive system monitoring

## 🏗️ Architecture

```
┌─────────────────┐    ┌──────────────────┐
│  HTTP Dashboard │◄──►│ MetricsCollector │
└─────────────────┘    └──────────────────┘
         │                       │
         ▼                       ▼
┌─────────────────┐    ┌──────────────────┐    ┌─────────────────┐
│   TCP Server    │◄──►│   LRUCache       │◄──►│ WAL & MMap      │
└─────────────────┘    └──────────────────┘    └─────────────────┘
         │                       │                       │
         ▼                       ▼                       ▼
┌─────────────────┐    ┌──────────────────┐    ┌─────────────────┐
│ Circuit Breaker │    │   Hash Ring      │    │ Node Discovery  │
└─────────────────┘    └──────────────────┘    └─────────────────┘
```

## 🚀 Quick Start

### Docker Deployment (Recommended)

```bash
# Clone and build
git clone <repo-url>
cd DistCache

# Deploy 3-node cluster
docker-compose up -d

# Check status
docker-compose ps

# View logs
docker-compose logs -f distcache-node1
```

### Manual Build

```bash
# Install dependencies (Ubuntu)
sudo apt-get install build-essential cmake libgtest-dev libmsgpack-dev

# Build
mkdir build && cd build
cmake ..
make -j$(nproc)

# Run tests
./tests/run_tests

# Run benchmarks
./benchmark/run_benchmark

# Start cache
./distcache
```

## 📊 Performance Benchmarks

```
[Benchmark] SET throughput: 273,306 ops/sec
[Benchmark] GET throughput: 897,644 ops/sec
[Benchmark] Average latency: 1.1μs
[Benchmark] Memory usage: 45MB (100K entries)
```

## 🧪 Testing

```bash
# Unit tests
docker run --rm distcache run_tests

# Benchmark tests
docker run --rm distcache run_benchmark

# Integration testing with Redis CLI
docker exec -it distcache-cli redis-cli -h distcache-node1
127.0.0.1:6379> SET mykey "Hello World"
OK
127.0.0.1:6379> GET mykey
"Hello World"
```

## 🔧 Configuration

Environment variables:
- `NODE_ID`: Unique node identifier
- `CLUSTER_NODES`: Comma-separated list of cluster nodes
- `CACHE_SIZE`: Maximum cache entries (default: 10000)
- `WAL_FILE`: Write-ahead log file path
- `SNAPSHOT_FILE`: Persistence snapshot file

## 📈 Monitoring

Access dashboards:
- Node 1: http://localhost:8080
- Node 2: http://localhost:8081  
- Node 3: http://localhost:8082

Metrics include:
- Request latency and throughput
- Cache hit/miss rates
- Circuit breaker state
- Node distribution statistics

