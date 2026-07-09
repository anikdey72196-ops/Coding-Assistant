# Stage 1: Build environment
FROM alpine:3.20 AS builder

# Install build dependencies
RUN apk add --no-cache build-base cmake git

# Set working directory
WORKDIR /app

# Copy CMake files first to cache dependencies if possible
COPY CMakeLists.txt .
COPY tests/CMakeLists.txt tests/

# Copy the source code
COPY src/ src/
COPY include/ include/
COPY tests/ tests/


# Configure and build the project
RUN cmake -B build -S . \
    -DCMAKE_BUILD_TYPE=Release \
    && cmake --build build --parallel

# Stage 2: Runtime environment
FROM alpine:3.20 AS runner

# Install runtime dependencies (libstdc++, libgcc)
RUN apk add --no-cache libstdc++ libgcc bash

# Set working directory
WORKDIR /app

# Copy the compiled binary from the builder stage
COPY --from=builder /app/build/my-assistant-cpp /app/my-assistant-cpp
COPY scripts/entrypoint.sh /app/entrypoint.sh

RUN chmod +x /app/entrypoint.sh

# Set the entrypoint
ENTRYPOINT ["/app/entrypoint.sh"]
