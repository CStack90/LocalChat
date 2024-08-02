# LocalChat

A school project about TCP and UDP communications.

# Project architecture
### Board
This directory is reserved for functions and main.c that we need for displaying every message of the chat.
### Client
This directory is reserved for functions and main.c that we need to send a message to the server, display all connected users, etc. It's the client side program.
### Server
This directory is reserved for functions and main.c that we need to manage requests from the client. It's the server side program.
### Utils
This directory is reserved for common functions that we will need in differents parts of the project.
### Samples
This directory contains example files studied in class to understand UDP and TCP connections
### Output
This directory is used to store executable files (board, client or server). It may contains files ending by '.o', they can be removed using: 
```console
make clean
```

# Build and run

Compile the whole project:
```console
make 
```

Execute client or server:
```console
make client
make board
make server
```
