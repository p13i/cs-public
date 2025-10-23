FROM docker.io/library/ubuntu:24.04@sha256:59a458b76b4e8896031cd559576eac7d6cb53a69b38ba819fb26518536368d86

# One apt step, cleaned, no recommends
RUN --mount=type=cache,target=/var/cache/apt \
    --mount=type=cache,target=/var/lib/apt \
    set -eux; \
    apt-get update; \
    apt-get install -y --no-install-recommends \
      ca-certificates curl gnupg clang lld g++ \
      build-essential git python3 sudo unzip zip zlib1g-dev \
      nodejs npm; \
    npm cache clean --force; rm -rf /root/.npm; \
    rm -rf /var/lib/apt/lists/*

# Print versions
RUN node -v && npm -v && npx -v

# Final clean (redundant but harmless here)
RUN rm -rf /var/lib/apt/lists/*

# Pre-install Bazelisk tool
RUN npm i -g @bazel/bazelisk

# Set working directory
WORKDIR /app

# Create non-root user 'myappuser' with UID 8877 and sudo privileges
RUN groupadd -g 8877 myappuser \
 && useradd -m -u 8877 -g 8877 -s /bin/bash myappuser

RUN usermod -aG sudo myappuser \
 && printf "myappuser ALL=(ALL) NOPASSWD:ALL\n" >/etc/sudoers.d/99-myappuser

# Set ownership of all files to 'myappuser'
RUN chown -R myappuser:myappuser /app

# Copy source files
COPY . .

# Run setup, build, and test — myappuser can use sudo if needed
USER myappuser

# Default command
CMD ["make", "help"]
