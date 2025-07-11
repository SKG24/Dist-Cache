FROM ubuntu:22.04 as builder

# Avoid interactive prompts
ENV DEBIAN_FRONTEND=noninteractive

# Install build dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    wget \
    libmsgpack-dev \
    && rm -rf /var/lib/apt/lists/*

# Install GTest from source (more reliable than system packages)
RUN cd /tmp && \
    git clone https://github.com/google/googletest.git && \
    cd googletest && \
    mkdir build && cd build && \
    cmake -DCMAKE_INSTALL_PREFIX=/usr/local .. && \
    make -j$(nproc) && \
    make install && \
    cd / && rm -rf /tmp/googletest

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
    netcat-openbsd \
    && rm -rf /var/lib/apt/lists/* \
    && groupadd -r distcache && useradd -r -g distcache distcache

# Copy built binaries - UPDATED NAMES
COPY --from=builder /app/build/dist-cache /usr/local/bin/
COPY --from=builder /app/build/run_benchmark /usr/local/bin/
COPY --from=builder /app/build/run_tests /usr/local/bin/

# Make binaries executable
RUN chmod +x /usr/local/bin/dist-cache /usr/local/bin/run_benchmark /usr/local/bin/run_tests

# Create data directory
RUN mkdir -p /data && chown distcache:distcache /data

# Switch to non-root user
USER distcache
WORKDIR /data

# Expose ports
EXPOSE 6379 8080

# Health check
HEALTHCHECK --interval=30s --timeout=10s --start-period=40s --retries=3 \
    CMD echo "PING" | timeout 5 nc localhost 6379 || exit 1

# Default command - UPDATED NAME
CMD ["dist-cache"]