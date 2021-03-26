#ifndef MY_CLIENT
#define MY_CLIENT

#include "connection.h"
#include "connectionclosedexception.h"
#include "protocol.h"
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
};

#endif