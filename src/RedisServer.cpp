#include <thread>
#include <iostream>
#include <vector>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <cstring>
#include <signal.h>
#include "../header_files/RedisServer.h"
#include "../header_files/RedisDatabase.h"
#include "../header_files/RedisCommandHandler.h"
// #pragma comment(lib, "ws2_32.lib")  // Link Winsock library

static MyRedisServer* globalServer = nullptr;

void signalHandler(int signum) {
    if (globalServer) {
        std::cout << "\nCaught signal " << signum << ", shutting down...\n";
        globalServer->shutdown();
    }
    // exit(signum);
}

void MyRedisServer::setupSignalHandler() {
    signal(SIGINT, signalHandler);
}



MyRedisServer::MyRedisServer(int port) : port(port), server_socket(INVALID_SOCKET), running(true) { // Constructor definition 
    globalServer = this;
        // Initialize Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed: " << WSAGetLastError() << "\n";
    }
    setupSignalHandler();
}

void MyRedisServer::shutdown() {
    if (RedisDatabase::getInstance().dump("dump.my_rdb"))
        std::cout << "Database Dumped to dump.my_rdb\n";
    else 
        std::cerr << "Error dumping database\n";
    running = false;
    if (server_socket != INVALID_SOCKET) {  
        closesocket(server_socket);
    }
    WSACleanup();  // Cleanup Winsock
    std::cout << "Server Shutdown Complete!\n";
}

void MyRedisServer::run() {
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET) {
        std::cerr << "Error Creating Server Socket\n";
        return;
    }

    int opt = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Error Binding Server Socket: " << WSAGetLastError() << "\n";
        closesocket(server_socket);
        return;
    }

    if (listen(server_socket, 10)  == SOCKET_ERROR) {
        std::cerr << "Error Listening On Server Socket: " << WSAGetLastError() << "\n";
        closesocket(server_socket);
        return;
    } 
    std::cout << "Redis Server Listening On Port " << port << "\n";
    std::vector<std::thread> threads;
    RedisCommandHandler cmdHandler;

    while (running) {
        SOCKET client_socket = accept(server_socket, nullptr, nullptr);
        if (client_socket == INVALID_SOCKET) {
            if (running) 
                std::cerr << "Error Accepting Client Connection: " << WSAGetLastError() << "\n";
            break;
        }
        threads.emplace_back([client_socket, &cmdHandler](){
            char buffer[1024];
            while (true) {
                memset(buffer, 0, sizeof(buffer));
                int bytes = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
                if (bytes <= 0) break;
                std::string request(buffer, bytes);
                std::string response = cmdHandler.processCommand(request);
                send(client_socket, response.c_str(), response.size(), 0);
            }
            closesocket(client_socket);
        });
    }
    
    for (auto& t : threads) {
        if (t.joinable()) t.join();
    }

    // Before shutdown, persist the database
    if (RedisDatabase::getInstance().dump("dump.my_rdb"))
        std::cout << "Database Dumped to dump.my_rdb\n";
    else 
        std::cerr << "Error dumping database\n";

}