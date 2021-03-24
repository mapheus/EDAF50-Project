#include "Server.h"

Server::Server(int port)
{

}

Server::~Server()
{

}

bool Server::isReady() const
{
    return false;
}

std::shared_ptr<Connection> Server::waitForActivity() const
{
    return std::make_shared();
}

void Server::registerConnection(const std::shared_ptr<Connection>& conn)
{

}

void Server::deregisterConnection(const std::shared_ptr<Connection>& conn)
{

}