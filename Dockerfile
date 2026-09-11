FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

# MinGW (Windows cross-compiler) aur Wine install karein
RUN apt-get update && apt-get install -y \
    g++-mingw-w64-x86-64 \
    wine64 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . .

# Windows exe build karein (Windows APIs wincrypt, ws2_32 automatically support honge)
RUN x86_64-w64-mingw32-g++ -O3 src/*.cpp -lws2_32 -lcrypt32 -o server.exe

EXPOSE 8080
ENV PORT=8080
ENV WINEDEBUG=-all

# Wine ke through Windows server execute karein
CMD ["wine64", "./server.exe"]
