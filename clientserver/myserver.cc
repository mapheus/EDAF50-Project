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
                        int end = mh.recvIntParameter();
                        if(end != static_cast<int>(Protocol::COM_END)) {throw new ConnectionClosedException;}

                        mh.sendIntParameter(static_cast<int>(Protocol::ANS_LIST_NG));
                        mh.sendIntParameter(newsgroups.size());
                        for(auto& e: newsgroups) {
                                mh.sendIntParameter(e.getId());
                                mh.sendStringParameter(e.getTitle());
                        }
                        mh.sendIntParameter(static_cast<int>(Protocol::ANS_END));
                        break;
                }
                case static_cast<int>(Protocol::COM_CREATE_NG):
                {
                        std::string groupName = mh.recvStringParameter();
                        int end = mh.recvIntParameter();
                        if(end != static_cast<int>(Protocol::COM_END)) {
                                std::cout << "connectipondlcosed" << std::endl;
                                throw new ConnectionClosedException;}

                        auto it = find_if(newsgroups.begin(), newsgroups.end(), [&groupName](NewsGroup& ng) {
                                return groupName == ng.getTitle();
                        });
                        mh.sendIntParameter(static_cast<int>(Protocol::ANS_CREATE_NG));
                        if(it == newsgroups.end()) 
                        {
                                mh.sendIntParameter(static_cast<int>(Protocol::ANS_ACK));
                                newsgroups.emplace_back(groupName);
                        } else {
                                mh.sendIntParameter(static_cast<int>(Protocol::ANS_NAK));
                                mh.sendIntParameter(static_cast<int>(Protocol::ERR_NG_ALREADY_EXISTS));
                        }
                        
                        mh.sendIntParameter(static_cast<int>(Protocol::ANS_END));
                        break;
                }
                case static_cast<int>(Protocol::COM_DELETE_NG):
                {
                        //COM_DELETE_NG num_p COM_END
                        //ANS_DELETE_NG [ANS_ACK | ANS_NAK ERR_NG_DOES_NOT_EXIST] ANS_END

                        int groupId = mh.recvIntParameter();
                        int end = mh.recvIntParameter();
                        if(end != static_cast<int>(Protocol::COM_END)) {throw new ConnectionClosedException;}
                        auto it = find_if(newsgroups.begin(), newsgroups.end(), [&groupId](NewsGroup& ng) {
                                return groupId == ng.getId();
                        });
                        mh.sendIntParameter(static_cast<int>(Protocol::ANS_DELETE_NG));
                        if(it == newsgroups.end()) 
                        {       
                                mh.sendIntParameter(static_cast<int>(Protocol::ANS_NAK));
                                mh.sendIntParameter(static_cast<int>(Protocol::ERR_NG_DOES_NOT_EXIST));
                                
                        } else {
                                newsgroups.erase(it);
                                mh.sendIntParameter(static_cast<int>(Protocol::ANS_ACK));
                                
                        }
                        mh.sendIntParameter(static_cast<int>(Protocol::ANS_END));
                        break;
                }
                case static_cast<int>(Protocol::COM_LIST_ART):
                {
                        //The reply contains the number of articles, followed by the identification numbers and titles of the articles
                        int id = mh.recvIntParameter();
                        int end = mh.recvIntParameter();
                        if(end != static_cast<int>(Protocol::COM_END)) {throw new ConnectionClosedException;}

                        mh.sendIntParameter(static_cast<int>(Protocol::ANS_LIST_ART));
                        auto it = find_if(newsgroups.begin(), newsgroups.end(), [&id](NewsGroup& ng) {
                                return id == ng.getId();
                        });
                        if(it == newsgroups.end()) 
                        {       
                                mh.sendIntParameter(static_cast<int>(Protocol::ANS_NAK));
                                mh.sendIntParameter(static_cast<int>(Protocol::ERR_NG_DOES_NOT_EXIST));
                                
                        } else {
                                mh.sendIntParameter(static_cast<int>(Protocol::ANS_ACK));
                                mh.sendIntParameter(it->getArticles().size());
                                for(auto& e: it->getArticles()) 
                                {
                                        mh.sendIntParameter(e->getId());
                                        mh.sendStringParameter(e->getTitle());
                                }
                        }
                        mh.sendIntParameter(static_cast<int>(Protocol::ANS_END));
                        break;
                }
                case static_cast<int>(Protocol::COM_CREATE_ART):
                {
                        //COM_CREATE_ART num_p string_p string_p string_p COM_END
                        //ANS_CREATE_ART [ANS_ACK | ANS_NAK ERR_NG_DOES_NOT_EXIST] ANS_END
                        int groupID = mh.recvIntParameter();
                        std::string title = mh.recvStringParameter();
                        std::string author = mh.recvStringParameter();
                        std::string text = mh.recvStringParameter();
                        int end = mh.recvIntParameter();
                        mh.sendIntParameter(static_cast<int>(Protocol::ANS_CREATE_ART));
                        if(end != static_cast<int>(Protocol::COM_END)) {throw new ConnectionClosedException;}

                        auto it = find_if(newsgroups.begin(), newsgroups.end(), [&groupID](NewsGroup& ng) {
                                return groupID == ng.getId();
                        });
                        if(it == newsgroups.end()) 
                        {       
                                mh.sendIntParameter(static_cast<int>(Protocol::ANS_NAK));
                                mh.sendIntParameter(static_cast<int>(Protocol::ERR_NG_DOES_NOT_EXIST));
                                
                        } else {
                                mh.sendIntParameter(static_cast<int>(Protocol::ANS_ACK));
                                it->addArticle(std::make_shared<Article>(title,author,text));
                        }
                        mh.sendIntParameter(static_cast<int>(Protocol::ANS_END));

                        break;
                }
                case static_cast<int>(Protocol::COM_DELETE_ART):
                {       //COM_DELETE_ART num_p num_p COM_END
                        //ANS_DELETE_ART [ANS_ACK |
                        //ANS_NAK [ERR_NG_DOES_NOT_EXIST | ERR_ART_DOES_NOT_EXIST]] ANS_END
                        int groupID = mh.recvIntParameter();
                        int articleID = mh.recvIntParameter();
                        int end = mh.recvIntParameter();
                        mh.sendIntParameter(static_cast<int>(Protocol::ANS_DELETE_ART));
                        if(end != static_cast<int>(Protocol::COM_END)) {throw new ConnectionClosedException;}

                        auto it = find_if(newsgroups.begin(), newsgroups.end(), [&groupID](NewsGroup& ng) {
                                return groupID == ng.getId();
                        });
                        if(it == newsgroups.end()) 
                        {       
                                mh.sendIntParameter(static_cast<int>(Protocol::ANS_NAK));
                                mh.sendIntParameter(static_cast<int>(Protocol::ERR_NG_DOES_NOT_EXIST));
                                
                        } else {
                                auto ita = it->getArticle(articleID);
                                if(ita == nullptr) {
                                        mh.sendIntParameter(static_cast<int>(Protocol::ANS_NAK));
                                        mh.sendIntParameter(static_cast<int>(Protocol::ERR_ART_DOES_NOT_EXIST));
                                } else 
                                {
                                        it->deleteArticle(articleID);
                                        mh.sendIntParameter(static_cast<int>(Protocol::ANS_ACK));
                                }
                        }
                        mh.sendIntParameter(static_cast<int>(Protocol::ANS_END));

                        break;
                }
                case static_cast<int>(Protocol::COM_GET_ART):
                {
                        int groupID = mh.recvIntParameter();
                        int articleID = mh.recvIntParameter();
                        int end = mh.recvIntParameter();
                        mh.sendIntParameter(static_cast<int>(Protocol::ANS_GET_ART));
                        if(end != static_cast<int>(Protocol::COM_END)) {throw new ConnectionClosedException;}
                        auto it = find_if(newsgroups.begin(), newsgroups.end(), [&groupID](NewsGroup& ng) {
                                return groupID == ng.getId();
                        });
                        if(it == newsgroups.end()) 
                        {       
                                mh.sendIntParameter(static_cast<int>(Protocol::ANS_NAK));
                                mh.sendIntParameter(static_cast<int>(Protocol::ERR_NG_DOES_NOT_EXIST));
                                
                        } else {
                                auto ita = it->getArticle(articleID);
                                if(ita == nullptr) {
                                        mh.sendIntParameter(static_cast<int>(Protocol::ANS_NAK));
                                        mh.sendIntParameter(static_cast<int>(Protocol::ERR_ART_DOES_NOT_EXIST));
                                } else 
                                {
                                        mh.sendIntParameter(static_cast<int>(Protocol::ANS_ACK));
                                        mh.sendStringParameter(ita->getTitle());
                                        mh.sendStringParameter(ita->getAuthor());
                                        mh.sendStringParameter(ita->getText());
                                }
                        }
                        mh.sendIntParameter(static_cast<int>(Protocol::ANS_END));

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
                                int nbr = mh.recvIntParameter();
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