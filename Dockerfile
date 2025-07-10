FROM ubuntu:22.04 as builder

# Install build dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    libgtest-dev \
    libmsgpack-dev \
    git \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /app

# Copy source code
COPY . .

# Build the project
RUN mkdir -p build && cd build && \
    cmake .. && \
    make -j$(nproc)

# Production stage
FROM ubuntu:22.04

# Install runtime dependencies
RUN apt-get update && apt-get install -y \
    libmsgpack-dev \
    && rm -rf /var/lib/apt/lists/* \
    && groupadd -r distcache && useradd -r -g distcache distcache

# Copy built binaries
COPY --from=builder /app/build/distcache /usr/local/bin/
COPY --from=builder /app/build/benchmark/run_benchmark /usr/local/bin/
COPY --from=builder /app/build/tests/run_tests /usr/local/bin/

# Create data directory
RUN mkdir -p /data && chown distcache:distcache /data

# Switch to non-root user
USER distcache
WORKDIR /data

# Expose ports
EXPOSE 6379 8080

# Health check
HEALTHCHECK --interval=30s --timeout=10s --start-period=5s --retries=3 \
    CMD echo "PING" > /dev/tcp/localhost/6379 || exit 1

# Default command
CMD ["distcache"]