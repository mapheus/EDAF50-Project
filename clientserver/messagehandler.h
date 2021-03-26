#ifndef MESSAGE_HANDLER
#define MESSAGE_HANDLER

/**
 * Message handler is the low-level message handler for messages to/from the
 * server. It handles transmission/reception of low-level message units.
 */
#include <memory>
#include <string>

#include "../connection.h"
#include "../connectionclosedexception.h"
#include "../protocolviolationexception.h"

using std::string;

class MessageHandler {
public:
    MessageHandler(const std::shared_ptr<Connection>& startConn);
    MessageHandler() {}
    void sendByte(int code);
    void sendCode(int code);
    void sendInt(int value);
    void sendIntParameter(int param);
    void sendStringParameter(string param);
    int recvByte();
    int recvCode();
    int recvInt();
    int recvIntParameter();
    string recvStringParameter();
    void changeConnection(const std::shared_ptr<Connection>& newConn);

private:
    std::shared_ptr<Connection> conn; // the connection


};

#endif