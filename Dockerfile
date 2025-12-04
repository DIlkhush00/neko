FROM ubuntu:latest
RUN apt-get update && \
    apt-get install -y \
        nasm \
        binutils \
        cmake \
        build-essential \
        nano \
    && rm -rf /var/lib/apt/lists/*
COPY . /app/
WORKDIR /app/
CMD ["sleep", "infinity"]