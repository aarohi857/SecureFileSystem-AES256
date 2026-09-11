FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y \
    build-essential \
    libssl-dev \
    sed \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . .

# Sabhi files se windows.h aur localtime_s ko automatically Linux compatible banana
RUN sed -i 's/#include <windows.h>/\/\/ #include <windows.h>/g' src/*.cpp src/*.h 2>/dev/null || true
RUN sed -i 's/localtime_s(&\([^,]*\), &\([^)]*\));/localtime_r(\&\2, \&\1);/g' src/*.cpp src/*.h 2>/dev/null || true

# Compile all source files
RUN g++ -O3 src/*.cpp -lssl -lcrypto -o server

EXPOSE 8080
ENV PORT=8080

CMD ["./server"]
