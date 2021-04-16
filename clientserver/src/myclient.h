#ifndef MY_CLIENT
#define MY_CLIENT

#include "server/connection.h"
#include "server/connectionclosedexception.h"
#include "server/protocol.h"
#include "messagehandler.h"

#include <cstdlib>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>


class MyClient {
public:
    MyClient(int argc, char* argv[]);
    int run();
    void inputHandler(int choice);

private:
    std::shared_ptr<Connection> conn;
    MessageHandler mh{};
    void printMenu();
    void listArticles(int ngID);
};

#endif