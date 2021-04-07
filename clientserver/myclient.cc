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

void MyClient::inputHandler(int choice) 
{
        switch(choice) 
        {
                case static_cast<int>(Protocol::COM_LIST_NG):
                {
                        mh.sendCode(static_cast<int>(Protocol::COM_END));
                        int ans = mh.recvCode();
                        if(ans != static_cast<int>(Protocol::ANS_LIST_NG)) {throw new ConnectionClosedException;}
                        int size = mh.recvIntParameter();
                        if(size != 0) 
                        {
                                std::cout << "newsgroup title , newsgroup id" << std::endl;
                        } else 
                        {
                                std::cout << "No newsgroups exists" << std::endl;
                        }
                        for(int i = 0; i < size ; i++) 
                        {
                                int id = mh.recvIntParameter();
                                std::string title = mh.recvStringParameter();
                                std::cout << title << " , " << id << std::endl;
                        }
                        int end = mh.recvCode();
                        if(end != static_cast<int>(Protocol::ANS_END)) {throw new ConnectionClosedException;}
                        break;
                }
                case static_cast<int>(Protocol::COM_CREATE_NG):
                {       
                        std::string title;
                        std::cout << "Write newsgroup title" << std::endl;
                        //std::getline(std::cin, title);
                        std::getline(std::cin, title);
                        mh.sendStringParameter(title);
                        mh.sendCode(static_cast<int>(Protocol::COM_END));
                        int ans = mh.recvCode();
                        if(ans != static_cast<int>(Protocol::ANS_CREATE_NG)) {throw new ConnectionClosedException;}
                        int ack = mh.recvCode(); // TODO depending on this receive one more
                        if(ack == static_cast<int>(Protocol::ANS_NAK)) 
                        {
                                int err = mh.recvCode();
                                std::cout << "Failed to create newsgroup" << std::endl;
                        }
                        else 
                        {
                                std::cout << "Created newsgroup " << title << std::endl;
                        }
                        int end = mh.recvCode();
                        if(end != static_cast<int>(Protocol::ANS_END)) {throw new ConnectionClosedException;}
                        break;

                }
                case static_cast<int>(Protocol::COM_DELETE_NG):
                {
                        //COM_DELETE_NG num_p COM_END
                        //ANS_DELETE_NG [ANS_ACK | ANS_NAK ERR_NG_DOES_NOT_EXIST] ANS_END
                        int id;
                        std::cout << "Write newsgroup id to remove" << std::endl;
                        std::cin >> id;
                        mh.sendIntParameter(id);
                        mh.sendCode(static_cast<int>(Protocol::COM_END));
                        int ans = mh.recvCode();
                        if(ans != static_cast<int>(Protocol::ANS_DELETE_NG)) {throw new ConnectionClosedException;}
                        int ack = mh.recvCode();
                        if(ack == static_cast<int>(Protocol::ANS_ACK)) 
                        {
                                std::cout << "Removed newsgroup" << std::endl;
                        }
                        else 
                        {
                                int err = mh.recvCode();
                                if (err != static_cast<int>(Protocol::ERR_NG_DOES_NOT_EXIST)) {throw new ConnectionClosedException;}
                                std::cout << "Failed to remove newsgroup" << std::endl;
                        }
                        int end = mh.recvCode();
                        if(end != static_cast<int>(Protocol::ANS_END)) {throw new ConnectionClosedException;}
                        break;
                }
                case static_cast<int>(Protocol::COM_LIST_ART):
                {       
                        int id;
                        std::cout << "Choose newsgroup id" << std::endl;
                        std::cin >> id;
                        mh.sendIntParameter(id);
                        mh.sendCode(static_cast<int>(Protocol::COM_END));
                        int ans = mh.recvCode();
                        if(ans != static_cast<int>(Protocol::ANS_LIST_ART)) {throw new ConnectionClosedException;}
                        int ack = mh.recvCode();
                        if(ack != static_cast<int>(Protocol::ANS_ACK)) 
                        {
                                int err = mh.recvCode();
                                if(err != static_cast<int>(Protocol::ERR_NG_DOES_NOT_EXIST)) {throw new ConnectionClosedException;}
                        } else 
                        {
                                int size = mh.recvIntParameter();
                                if(size != 0) 
                                {
                                        std::cout << "Article title , Article id" << std::endl;
                                } else 
                                {
                                        std::cout << "No Articles exists" << std::endl;
                                }
                                for(int i = 0; i < size ; i++) 
                                {
                                        int id = mh.recvIntParameter();
                                        std::string title = mh.recvStringParameter();
                                        std::cout << title << " , " << id << std::endl;
                                }
                        }
                        int end = mh.recvCode();
                        if(end != static_cast<int>(Protocol::ANS_END)) {throw new ConnectionClosedException;}
                        break;
                }
                case static_cast<int>(Protocol::COM_CREATE_ART):
                {
                        //COM_CREATE_ART num_p string_p string_p string_p COM_END
                        //ANS_CREATE_ART [ANS_ACK | ANS_NAK ERR_NG_DOES_NOT_EXIST] ANS_END
                        int id;
                        std::cout << "Choose newsgroup id" << std::endl;
                        std::cin >> id;
                        std::cin.ignore();
                        std::cout << "write title" << std::endl;
                        std::string title;
                        std::getline(std::cin, title);
                        std::cout << "write author" << std::endl;
                        std::string author;
                        std::getline(std::cin, author);
                        std::cout << "write article text" << std::endl;
                        std::string text;
                        std::getline(std::cin, text);
                        mh.sendIntParameter(id);
                        mh.sendStringParameter(title);
                        mh.sendStringParameter(author);
                        mh.sendStringParameter(text);
                        mh.sendCode(static_cast<int>(Protocol::COM_END));
                        int ans = mh.recvCode();
                        if(ans != static_cast<int>(Protocol::ANS_CREATE_ART)) {throw new ConnectionClosedException;}
                        int ack = mh.recvCode();
                        if(ack == static_cast<int>(Protocol::ANS_ACK)) 
                        {
                                std::cout << "Created article" << std::endl;
                        }
                        else 
                        {
                                int err = mh.recvCode();
                                if (err != static_cast<int>(Protocol::ERR_NG_DOES_NOT_EXIST)) {throw new ConnectionClosedException;}
                                std::cout << "Failed to find newsgroup" << std::endl;
                        }
                        int end = mh.recvCode();
                        if(end != static_cast<int>(Protocol::ANS_END)) {throw new ConnectionClosedException;}
                        break;
                }
                case static_cast<int>(Protocol::COM_DELETE_ART):
                {
                        int groupID;
                        std::cout << "Choose newsgroup id" << std::endl;
                        std::cin >> groupID;
                        mh.sendIntParameter(groupID);
                        int articleID;
                        std::cout << "Choose article id" << std::endl;
                        std::cin >> articleID;
                        mh.sendIntParameter(articleID);
                        mh.sendCode(static_cast<int>(Protocol::COM_END));
                        int ans = mh.recvCode();
                        if(ans != static_cast<int>(Protocol::ANS_DELETE_ART)) {throw new ConnectionClosedException;}
                        int ack = mh.recvCode();
                        if(ack == static_cast<int>(Protocol::ANS_ACK)) 
                        {
                                std::cout << "Removed article" << std::endl;
                        } else {
                                int err = mh.recvCode();
                                if(err == static_cast<int>(Protocol::ERR_NG_DOES_NOT_EXIST)) {
                                     std::cout << "Failed to find newsgroup" << std::endl;   
                                } else {
                                        std::cout << "Failed to find article" << std::endl;   
                                }
                        }
                        int end = mh.recvCode();
                        if(end != static_cast<int>(Protocol::ANS_END)) {throw new ConnectionClosedException;}
                        break;
                }
                case static_cast<int>(Protocol::COM_GET_ART):
                {
                        int groupID;
                        std::cout << "Choose newsgroup id" << std::endl;
                        std::cin >> groupID;
                        mh.sendIntParameter(groupID);
                        int articleID;
                        std::cout << "Choose article id" << std::endl;
                        std::cin >> articleID;
                        mh.sendIntParameter(articleID);
                        mh.sendCode(static_cast<int>(Protocol::COM_END));
                        int ans = mh.recvCode();
                        if(ans != static_cast<int>(Protocol::ANS_GET_ART)) {throw new ConnectionClosedException;}
                        int ack = mh.recvCode();
                        if(ack == static_cast<int>(Protocol::ANS_ACK)) 
                        {
                                std::cout << "title: " << mh.recvStringParameter() << std::endl;
                                std::cout << "author: " << mh.recvStringParameter() << std::endl;
                                std::cout << "text: " << std::endl;
                                std::cout << mh.recvStringParameter() << std::endl;
                        } else {
                                int err = mh.recvCode();
                                if(err == static_cast<int>(Protocol::ERR_NG_DOES_NOT_EXIST)) {
                                     std::cout << "Failed to find newsgroup" << std::endl;   
                                } else {
                                        std::cout << "Failed to find article" << std::endl;   
                                }
                        }
                        int end = mh.recvCode();
                        if(end != static_cast<int>(Protocol::ANS_END)) {throw new ConnectionClosedException;}
                        break;
                }
                default:
                {
                        throw new ConnectionClosedException;
                        break;
                }
        }
}


int MyClient::run()
{
        int nbr;
        //std::cout << "Type a number: " << std::endl;
        
        while(true) {
                try {
                        std::cout << "Type a number: " << std::endl;
                        std::cin >> nbr;
                        std::cin.ignore();
                        mh.sendCode(nbr);
                        inputHandler(nbr);
                        /*
                        if(nbr == 2) { // TODO add input handler
                                mh.sendStringParameter("First");
                        }
                        mh.sendIntParameter(8);
                        int ans = mh.recvInt();
                        int ack = mh.recvInt(); // TODO depending on this receive one more
                        int end = mh.recvInt();
                        std::cout << "ans: " << ans << std::endl;
                        std::cout << "Ack: " << ack << std::endl;
                        std::cout << "end: " << end << std::endl;
                        */
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
