#include "Connection.h"

Connection::Connection(const char* host, int port)
{

}

Connection::Connection()
{

}

Connection::~Connection()
{

}

bool Connection::isConnected() const
{
    return false;
}

void Connection::write(unsigned char ch) const
{

}

unsigned char Connection::read() const
{
    return 'a';
}