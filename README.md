# My Redis Server

A custom Redis server implementation built from scratch. This project aims to replicate core Redis functionality while providing a clean, efficient codebase that's easy to understand and extend.

## Overview

This Redis server implementation offers in-memory data storage with support for key Redis data structures and commands. It's designed for educational purposes and practical use cases that require a lightweight in-memory database with Redis-compatible APIs.

## Features

- In-memory key-value store with persistent storage options
- Support for core Redis data structures: Strings, Lists, Sets, Hashes, and Sorted Sets
- TCP server handling concurrent client connections
- Redis protocol (RESP) implementation
- Command parsing and execution framework

## Installation

### Prerequisites

- C/C++ compiler (GCC, Clang, or MSVC)
- CMake (version 3.10+)
- Make or equivalent build system

### Building from Source

Clone the repository and build:

```bash
git clone https://github.com/Achintya512/My-Redis-Server.git
cd My-Redis-Server
make
```

## Usage

Start the server:

```bash
./redis-server [port]
```

Default port is 6379 if not specified.

Connect using Redis CLI or any Redis client:

```bash
redis-cli -p 6379
```

# Supported Redis Commands

This C++ Redis-like database implementation supports the following commands:

## General Commands
- `FLUSHALL` - Remove all keys from all databases

## Key/Value Operations
- `SET key value` - Set key to hold string value
- `GET key` - Get the value of key
- `KEYS` - Returns all keys in the database
- `TYPE key` - Return the type of value stored at key
- `DEL key` - Delete a key
- `EXPIRE key seconds` - Set a key's time to live in seconds
- `RENAME key newkey` - Rename a key

## List Operations
- `LGET key` - Get all elements of a list
- `LLEN key` - Get the length of a list
- `LPUSH key value` - Insert value at the head of a list
- `RPUSH key value` - Insert value at the tail of a list
- `LPOP key` - Remove and get the first element in a list
- `RPOP key` - Remove and get the last element in a list
- `LREM key count value` - Remove elements from a list
- `LINDEX key index` - Get an element from a list by its index
- `LSET key index value` - Set the value of an element in a list by its index

## Hash Operations
- `HSET key field value` - Set the string value of a hash field
- `HGET key field` - Get the value of a hash field
- `HEXISTS key field` - Determine if a hash field exists
- `HDEL key field` - Delete one hash field
- `HGETALL key` - Get all the fields and values in a hash
- `HKEYS key` - Get all the fields in a hash
- `HVALS key` - Get all the values in a hash
- `HLEN key` - Get the number of fields in a hash
- `HMSET key field value [field value ...]` - Set multiple hash fields to multiple values

## Persistence Operations
- `DUMP filename` - Serialize and save the database to a file
- `LOAD filename` - Deserialize and load the database from a file

## Internal Functions
- `purgeExpired` - Remove expired keys from the database (automatically called)

## Implementation Details

### Architecture

```
+------------------+
| Client Interface |
+--------+---------+
         |
+--------v---------+
| Command Processor|
+--------+---------+
         |
+--------v---------+     +---------------+
| Data Structures  +---->+ Persistence   |
+------------------+     +---------------+
```

The server follows a modular design with these main components:

1. **Network Layer** - Handles client connections using non-blocking I/O
2. **Protocol Parser** - Implements RESP (Redis Serialization Protocol)
3. **Command Processor** - Interprets and executes Redis commands
4. **Data Store** - Implements the in-memory data structures
5. **Persistence Module** - Handles data persistence (snapshots or logs)



## Limitations

- Currently supports single database mode (no SELECT command)
- Limited transaction support
- No clustering or replication features
- Some advanced Redis commands not implemented

## Future Work

- Add pub/sub messaging functionality
- Implement LUA scripting
- Add more advanced data structures (Bitmaps, HyperLogLogs)
- Implement AOF persistence mechanism
- Add replication support