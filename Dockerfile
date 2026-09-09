FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    build-essential \
    g++ \
    libssl-dev

WORKDIR /app
COPY . .

RUN g++ -O3 src/*.cpp -lssl -lcrypto -o server

EXPOSE 8080
CMD ["./server"]
