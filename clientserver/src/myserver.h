#ifndef MY_SERVER
#define MY_SERVER

#include "./server/connection.h"
#include "./server/connectionclosedexception.h"
#include "./server/server.h"
#include "./server/protocol.h"
#include "messagehandler.h"
#include "newsgroup.h"
#include "storage.h"

#include <cstdlib>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

struct ArticleInfo
{
    std::shared_ptr<Article> article;
    int groupID;
    int articleID;
    bool success;
};

class MyServer {
public:
    MyServer(int argc, char* argv[]);
    Server init(int argc, char* argv[]);
    void run();
    void inputHandler(int choice);

private:
    ArticleInfo GetArticleAndSendInfo(Protocol prot);

private:
    std::unique_ptr<Storage> storage;
    std::unique_ptr<Server> server;
    MessageHandler mh{};
};

#endif