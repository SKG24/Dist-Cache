FROM ubuntu:22.04 as builder

# Avoid interactive prompts
ENV DEBIAN_FRONTEND=noninteractive

# Install build dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    pkg-config \
    git \
    libmsgpack-dev \
    libgtest-dev \
    && rm -rf /var/lib/apt/lists/*

# Build Google Test (required for Ubuntu 22.04)
RUN cd /usr/src/gtest && \
    cmake . && \
    make && \
    cp lib/*.a /usr/lib && \
    cd /usr/src/gmock && \
    cmake . && \
    make && \
    cp lib/*.a /usr/lib

# Set working directory
WORKDIR /app

# Copy source code
COPY . .

# Build the project
RUN mkdir -p build && cd build && \
    cmake -DCMAKE_BUILD_TYPE=Release .. && \
    make -j$(nproc) && \
    ls -la

# Production stage
FROM ubuntu:22.04

# Install runtime dependencies
RUN apt-get update && apt-get install -y \
    libmsgpack-dev \
    netcat \
    && rm -rf /var/lib/apt/lists/* \
    && groupadd -r distcache && useradd -r -g distcache distcache

# Copy built binaries
COPY --from=builder /app/build/distcache /usr/local/bin/
COPY --from=builder /app/build/run_benchmark /usr/local/bin/
COPY --from=builder /app/build/run_tests /usr/local/bin/

# Create data directory
RUN mkdir -p /data && chown distcache:distcache /data

# Switch to non-root user
USER distcache
WORKDIR /data

# Expose ports
EXPOSE 6379 8080

# Health check
HEALTHCHECK --interval=30s --timeout=10s --start-period=5s --retries=3 \
    CMD echo "PING" | nc -w 1 localhost 6379 || exit 1

# Default command
CMD ["distcache"]