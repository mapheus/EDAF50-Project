#ifndef CONNECTION_H
#define CONNECTION_H

/* A ConnectionClosedException is thrown when a connection is closed */
struct ConnectionClosedException {};

/* A Connection object represents a connection (a socket) */
class Connection {
    friend class Server;
    public:
    /* Establishes a connection to the computer ’host’ via
    the port ’port’ */
    Connection(const char* host, int port);
    /* Creates a Connection object which will be initialized
    by the server */
    Connection();
    /* Closes the connection */
    virtual ~Connection();
    /* Returns true if the connection has been established */
    bool isConnected() const;
    /* Writes a character */
    void write(unsigned char ch) const;
    /* Reads a character */
    unsigned char read() const;
    /* Connection cannot be copied */
    Connection(const Connection&) = delete;
    Connection& operator=(const Connection&) = delete;
    Connection(Connection&&);
    Connection& operator=(Connection&&) = delete;
};

#endif