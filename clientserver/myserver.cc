#include "myserver.h"

#include "storagememory.h"
#include "storagedisk.h"

#define MEMORY

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

        #ifdef MEMORY
        storage = std::make_unique<StorageMemory>();
        #else
        storage = std::make_unique<StorageDisk>();
        #endif
}

void MyServer::inputHandler(int choice) 
{
        switch(choice) 
        {
                case static_cast<int>(Protocol::COM_LIST_NG):
                {
                        int end = mh.recvCode();
                        if(end != static_cast<int>(Protocol::COM_END)) {throw new ConnectionClosedException;}

                        mh.sendCode(static_cast<int>(Protocol::ANS_LIST_NG));
                        std::vector<std::shared_ptr<NewsGroup>> newsgroups = storage->ListNewsGroups();
                        mh.sendIntParameter(newsgroups.size());
                        for(auto& e: newsgroups) {
                                mh.sendIntParameter(e->getId());
                                mh.sendStringParameter(e->getTitle());
                        }
                        mh.sendCode(static_cast<int>(Protocol::ANS_END));
                        break;
                }
                case static_cast<int>(Protocol::COM_CREATE_NG):
                {
                        std::string groupName = mh.recvStringParameter();
                        int end = mh.recvCode();
                        if(end != static_cast<int>(Protocol::COM_END)) {
                                std::cout << "connectipondlcosed" << std::endl;
                                throw new ConnectionClosedException;}

                        bool created = storage->CreateNewsGroup(groupName);
                        mh.sendCode(static_cast<int>(Protocol::ANS_CREATE_NG));
                        if(created) 
                        {
                                mh.sendCode(static_cast<int>(Protocol::ANS_ACK));
                        } else {
                                mh.sendCode(static_cast<int>(Protocol::ANS_NAK));
                                mh.sendCode(static_cast<int>(Protocol::ERR_NG_ALREADY_EXISTS));
                        }
                        
                        mh.sendCode(static_cast<int>(Protocol::ANS_END));
                        break;
                }
                case static_cast<int>(Protocol::COM_DELETE_NG):
                {
                        //COM_DELETE_NG num_p COM_END
                        //ANS_DELETE_NG [ANS_ACK | ANS_NAK ERR_NG_DOES_NOT_EXIST] ANS_END

                        int groupId = mh.recvIntParameter();
                        int end = mh.recvCode();
                        if(end != static_cast<int>(Protocol::COM_END)) {throw new ConnectionClosedException;}
                        bool deleted = storage->DeleteNewsGroup(groupId);
                        mh.sendCode(static_cast<int>(Protocol::ANS_DELETE_NG));
                        if(!deleted) 
                        {       
                                mh.sendCode(static_cast<int>(Protocol::ANS_NAK));
                                mh.sendCode(static_cast<int>(Protocol::ERR_NG_DOES_NOT_EXIST));
                                
                        } else {
                                mh.sendCode(static_cast<int>(Protocol::ANS_ACK));    
                        }
                        mh.sendCode(static_cast<int>(Protocol::ANS_END));
                        break;
                }
                case static_cast<int>(Protocol::COM_LIST_ART):
                {
                        //The reply contains the number of articles, followed by the identification numbers and titles of the articles
                        int id = mh.recvIntParameter();
                        int end = mh.recvCode();
                        if(end != static_cast<int>(Protocol::COM_END)) {throw new ConnectionClosedException;}

                        std::shared_ptr<NewsGroup> ng = storage->GetNewsGroup(id);
                        mh.sendCode(static_cast<int>(Protocol::ANS_LIST_ART));
                        if(ng == nullptr) 
                        {       
                                mh.sendCode(static_cast<int>(Protocol::ANS_NAK));
                                mh.sendCode(static_cast<int>(Protocol::ERR_NG_DOES_NOT_EXIST));
                                
                        } else {
                                mh.sendCode(static_cast<int>(Protocol::ANS_ACK));
                                mh.sendIntParameter(ng->getArticles().size());
                                for(auto& e: ng->getArticles()) 
                                {
                                        mh.sendIntParameter(e->getId());
                                        mh.sendStringParameter(e->getTitle());
                                }
                        }
                        mh.sendCode(static_cast<int>(Protocol::ANS_END));
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
                        int end = mh.recvCode();
                        mh.sendCode(static_cast<int>(Protocol::ANS_CREATE_ART));
                        if(end != static_cast<int>(Protocol::COM_END)) {throw new ConnectionClosedException;}

                        bool created = storage->CreateArticle(groupID, title, author, text);
                        if(!created) 
                        {       
                                mh.sendCode(static_cast<int>(Protocol::ANS_NAK));
                                mh.sendCode(static_cast<int>(Protocol::ERR_NG_DOES_NOT_EXIST));
                                
                        } else {
                                mh.sendCode(static_cast<int>(Protocol::ANS_ACK));
                        }
                        mh.sendCode(static_cast<int>(Protocol::ANS_END));

                        break;
                }
                case static_cast<int>(Protocol::COM_DELETE_ART):
                {       //COM_DELETE_ART num_p num_p COM_END
                        //ANS_DELETE_ART [ANS_ACK |
                        //ANS_NAK [ERR_NG_DOES_NOT_EXIST | ERR_ART_DOES_NOT_EXIST]] ANS_END
                        int groupID = mh.recvIntParameter();
                        int articleID = mh.recvIntParameter();
                        int end = mh.recvCode();
                        mh.sendCode(static_cast<int>(Protocol::ANS_DELETE_ART));
                        if(end != static_cast<int>(Protocol::COM_END)) {throw new ConnectionClosedException;}

                        std::shared_ptr<NewsGroup> ng = storage->GetNewsGroup(groupID);
                        if(ng == nullptr) 
                        {       
                                mh.sendCode(static_cast<int>(Protocol::ANS_NAK));
                                mh.sendCode(static_cast<int>(Protocol::ERR_NG_DOES_NOT_EXIST));
                                
                        } 
                        else
                        {
                                std::shared_ptr<Article> article = ng->getArticle(articleID);
                                if(article == nullptr)
                                {
                                        mh.sendCode(static_cast<int>(Protocol::ANS_NAK));
                                        mh.sendCode(static_cast<int>(Protocol::ERR_ART_DOES_NOT_EXIST));
                                }
                                else 
                                {
                                        storage->DeleteArticle(groupID, articleID);
                                        mh.sendCode(static_cast<int>(Protocol::ANS_ACK));
                                }
                        } 
                        mh.sendCode(static_cast<int>(Protocol::ANS_END));

                        break;
                }
                case static_cast<int>(Protocol::COM_GET_ART):
                {
                        int groupID = mh.recvIntParameter();
                        int articleID = mh.recvIntParameter();
                        int end = mh.recvCode();
                        mh.sendCode(static_cast<int>(Protocol::ANS_GET_ART));
                        if(end != static_cast<int>(Protocol::COM_END)) {throw new ConnectionClosedException;}
                        std::shared_ptr<NewsGroup> ng = storage->GetNewsGroup(groupID);
                        if(ng == nullptr) 
                        {       
                                mh.sendCode(static_cast<int>(Protocol::ANS_NAK));
                                mh.sendCode(static_cast<int>(Protocol::ERR_NG_DOES_NOT_EXIST));
                        } 
                        else
                        {
                                auto article = ng->getArticle(articleID);
                                if(article == nullptr) {
                                        mh.sendCode(static_cast<int>(Protocol::ANS_NAK));
                                        mh.sendCode(static_cast<int>(Protocol::ERR_ART_DOES_NOT_EXIST));
                                } 
                                else 
                                {
                                        mh.sendCode(static_cast<int>(Protocol::ANS_ACK));
                                        mh.sendStringParameter(article->getTitle());
                                        mh.sendStringParameter(article->getAuthor());
                                        mh.sendStringParameter(article->getText());
                                }
                        }
                        mh.sendCode(static_cast<int>(Protocol::ANS_END));

                        break;
                }
                default:
                {
                        std::cout << "Default closed connection" << std::endl;
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
                                int nbr = mh.recvCode();
                                std::cout << "number: " << nbr << std::endl;
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