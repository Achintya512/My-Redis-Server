#include <iostream>
#include "../header_files/RedisServer.h"
#include <thread>
#include <chrono>
int main(int argc, char* argv[]) {
    int port = 6379; // default
    if (argc >=2) port = std::stoi(argv[1]); 

    MyRedisServer server(port);
    // Backgroung persistance: dump the database every 300 seconds. (5 * 60 seconds save database)
    std::thread persistanceThread([](){
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(300));
        }
    });
    persistanceThread.detach();
    // Start the server (this will block and keep the program running)
    server.run();
    return 0;
}