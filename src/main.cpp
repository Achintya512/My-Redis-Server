#include <iostream>
#include "../header_files/RedisServer.h"
#include "../header_files/RedisDatabase.h"
#include <thread>
#include <chrono>
int main(int argc, char* argv[]) {
    int port = 6379; // default
    if (argc >=2) port = std::stoi(argv[1]); 

    if (RedisDatabase::getInstance().load("dump.my_rdb"))
        std::cout << "Database Loaded From dump.my_rdb\n";
    else 
        std::cout << "No dump found or load failed; starting with an empty database.\n";

    MyRedisServer server(port);
    // Backgroung persistance: dump the database every 300 seconds. (5 * 60 seconds save database)
    std::thread persistanceThread([](){
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(10));
            if (!RedisDatabase::getInstance().dump("dump.my_rdb"))
                std::cerr << "Error Dumping Database\n";
            else 
                std::cout << "Database Dumped to dump.my_rdb\n";
        }
    });
    persistanceThread.detach();
    // Start the server (this will block and keep the program running)
    server.run();
    return 0;
}