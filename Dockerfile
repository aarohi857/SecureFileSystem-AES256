FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

# MinGW aur Wine install karein
RUN apt-get update && apt-get install -y \
    g++-mingw-w64-x86-64 \
    wine64 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . .

# -std=c++17 flag add kiya hai filesystem support ke liye
RUN x86_64-w64-mingw32-g++ -std=c++17 -O3 src/*.cpp -lws2_32 -lcrypt32 -lstdc++fs -o server.exe

EXPOSE 8080
ENV PORT=8080
ENV WINEDEBUG=-all

CMD ["wine64", "./server.exe"]
