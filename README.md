# Parallel File Transfer using Sockets and OpenMP

This project demonstrates parallel file transfer from multiple clients to a server using socket programming in C++ with OpenMP. The server accepts file uploads from clients in parallel, improving performance and efficiency when handling large file transfers. The implementation supports error handling, file integrity checks, and performance evaluation.

## Features

- **Parallel File Transfer:** Multiple clients can send files simultaneously to the server.
- **Socket Programming:** Uses TCP sockets for reliable file transfer between clients and the server.
- **OpenMP Parallelism:** Uses OpenMP to handle multiple client connections in parallel.
- **File Handling:** Efficient handling of large files with built-in error checking.

## Files

- `client.cpp`: Contains the code for the client-side program that connects to the server and sends files.
- `server.cpp`: Contains the code for the server-side program that handles multiple client connections in parallel using threads.

## How It Works

1. **Server:** The server listens for incoming client connections. When a client connects, a new thread is spawned to handle the file transfer using OpenMP, ensuring parallel execution.
2. **Client:** Each client connects to the server, specifies the file to send, and transmits it over the network.
3. **Multi-client Support:** The server handles multiple client connections in parallel to improve efficiency.

## Technologies Used

- **C++**
- **OpenMP** for parallelism
- **Socket Programming** for client-server communication

## Usage

1. **Compile the server and client:**
    ```bash
    g++ -o server.exe server.cpp -fopenmp
    g++ -o client.exe client.cpp -fopenmp
    ```

2. **Start the server:**
    ```bash
    ./server.exe
    ```

3. **Run the client(s) to send files:**
    ```bash
    ./client.exe
    ```

## Performance Evaluation

The project evaluates the performance of serial versus parallel file transfers, demonstrating the benefits of parallelism on multi-core systems.
