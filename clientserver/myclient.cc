/* myclient.cc: sample client program */
#include "connection.h"
#include "connectionclosedexception.h"
#include "myclient.h"

#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>

/* Creates a client for the given args, if possible.
 * Otherwise exits with error code.
 */
MyClient::MyClient(int argc, char* argv[])
{
        if (argc != 3) 
        {
                std::cerr << "Usage: myclient host-name port-number" << std::endl;
                exit(1);
        }

        int port = -1;
        try {
                port = std::stoi(argv[2]);
        } catch (std::exception& e) {
                std::cerr << "Wrong port number. " << e.what() << std::endl;
                exit(2);
        }
        conn = std::make_shared<Connection>(argv[1], port);
        if (!conn->isConnected()) 
        {
                std::cerr << "Connection attempt failed" << std::endl;
                exit(3);
        }
        mh = MessageHandler(conn);
}

int MyClient::run()
{
        int nbr;
        std::cout << "Type a number: " << std::endl;
        
        while(std::cin >> nbr) {
                try {
                        mh.sendInt(nbr);
                        if(nbr == 2) { // TODO add input handler
                                mh.sendStringParameter("First");
                        }
                        mh.sendInt(8);
                        int ans = mh.recvInt();
                        int ack = mh.recvInt(); // TODO depending on this receive one more
                        int end = mh.recvInt();
                        std::cout << "ans: " << ans << std::endl;
                        std::cout << "Ack: " << ack << std::endl;
                        std::cout << "end: " << end << std::endl;
                } catch (ConnectionClosedException&) {
                        std::cout << " no reply from server. Exiting." << std::endl;
                        return 1;
                }
        }
        std::cout << "\nexiting.\n";
        return 0;
}

int main(int argc, char* argv[])
{
        MyClient client(argc, argv);
        return client.run();
}
