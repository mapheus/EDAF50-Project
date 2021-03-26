#include "myserver.h"

MyServer::MyServer(int argc, char* argv[])
{
        if (argc != 2) 
        {
                std::cerr << "Usage: myserver port-number" << std::endl;
                exit(1);
        }

        int port = -1;
        try {
                port = std::stoi(argv[1]);
        } catch (std::exception& e) {
                std::cerr << "Wrong format for port number. " << e.what() << std::endl;
                exit(2);
        }

        server = std::make_unique<Server>(port);
        if (!server->isReady()) 
        {
                std::cerr << "Server initialization error." << std::endl;
                exit(3);
        }
}

void MyServer::inputHandler(int choice) 
{
        switch(choice) 
        {
                case static_cast<int>(Protocol::COM_LIST_NG):
                {
                        std::cout << "123" << std::endl;
                        int end = mh.recvInt();
                        if(end != 8) {throw new ConnectionClosedException;}

                        mh.sendInt(static_cast<int>(Protocol::ANS_LIST_NG));
                        mh.sendInt(newsgroups.size());
                        for(auto& e: newsgroups) {
                                mh.sendInt(e.getId());
                                mh.sendStringParameter(e.getTitle());
                        }
                        mh.sendInt(static_cast<int>(Protocol::ANS_END));
                        break;
                }
                case static_cast<int>(Protocol::COM_CREATE_NG):
                {
                        std::string groupName = mh.recvStringParameter();
                        int end = mh.recvInt();
                        if(end != 8) {throw new ConnectionClosedException;}

                        auto it = find_if(newsgroups.begin(), newsgroups.end(), [&groupName](NewsGroup& ng) {
                                return groupName == ng.getTitle();
                        });
                        mh.sendInt(static_cast<int>(Protocol::ANS_CREATE_NG));
                        if(it == newsgroups.end() && newsgroups.size() != 0) 
                        {
                                mh.sendInt(static_cast<int>(Protocol::ANS_NAK));
                                mh.sendInt(static_cast<int>(Protocol::ERR_NG_ALREADY_EXISTS));
                        } else {
                                mh.sendInt(static_cast<int>(Protocol::ANS_ACK));
                                newsgroups.emplace_back(groupName);
                        }
                        
                        mh.sendInt(static_cast<int>(Protocol::ANS_END));
                        break;
                }
                case static_cast<int>(Protocol::COM_DELETE_NG):
                {
                        


                        mh.recvInt();
                        break;
                }
                case static_cast<int>(Protocol::COM_LIST_ART):
                {
                        mh.recvInt();
                        break;
                }
                case static_cast<int>(Protocol::COM_CREATE_ART):
                {
                        mh.recvInt();
                        break;
                }
                case static_cast<int>(Protocol::COM_DELETE_ART):
                {
                        mh.recvInt();
                        break;
                }
                case static_cast<int>(Protocol::COM_GET_ART):
                {
                        mh.recvInt();
                        break;
                }
                default:
                {
                        throw new ConnectionClosedException;
                        break;
                }
        }
}

void MyServer::run() {
        while (true) 
        {
                auto conn = server->waitForActivity();
                mh.changeConnection(conn);
                if (conn != nullptr) 
                {
                        try {
                                int nbr = mh.recvInt();
                                inputHandler(nbr);

                                
                                //writeString(conn, result);
                        } catch (ConnectionClosedException&) {
                                server->deregisterConnection(conn);
                                std::cout << "Client closed connection" << std::endl;
                        }
                } else {
                        conn = std::make_shared<Connection>();
                        server->registerConnection(conn);
                        std::cout << "New client connects" << std::endl;
                }
        }
}


int main(int argc, char* argv[])
{
        MyServer server(argc, argv);
        server.run();
        return 0;
}