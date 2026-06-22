# ============================================
# KaliShell - Universal Docker Build
# Works on: Linux, Windows (Docker), macOS
# ============================================

# Build stage
FROM gcc:13 AS builder

# Install dependencies
RUN apt-get update && apt-get install -y \
    libsqlite3-dev \
    make \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /build

# Copy source files
COPY . .

# Build KaliShell
RUN make clean && make release

# Runtime stage
FROM debian:bookworm-slim

# Install runtime dependencies
RUN apt-get update && apt-get install -y \
    libsqlite3-0 \
    && rm -rf /var/lib/apt/lists/*

# Copy built binary
COPY --from=builder /build/build/kalishell /usr/local/bin/kalishell

# Copy config and tools
COPY config/ /etc/kalishell/
COPY tools/ /usr/share/kalishell/tools/
COPY scripts/ /usr/share/kalishell/scripts/

# Create workspace directory
RUN mkdir -p /workspace

# Set working directory
WORKDIR /workspace

# Default entrypoint
ENTRYPOINT ["kalishell"]
CMD ["--help"]
