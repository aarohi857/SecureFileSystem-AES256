FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    g++-mingw-w64-x86-64 \
    wine64 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . .

# Added -Wl,--allow-multiple-definition to bypass duplicate function definitions
RUN x86_64-w64-mingw32-g++ -std=c++17 -O3 src/*.cpp -Wl,--allow-multiple-definition -lws2_32 -lcrypt32 -lstdc++fs -o server.exe

EXPOSE 8080
ENV PORT=8080
ENV WINEDEBUG=-all

CMD ["wine64", "./server.exe"]
