/* myclient.cc: sample client program */
#include "server/connection.h"
#include "server/connectionclosedexception.h"
#include "myclient.h"

#include <cstdlib>
#include <iostream>
#include <limits>
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

void MyClient::listArticles(int ngID) {
        mh.sendCode(4);
        mh.sendIntParameter(ngID);
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
                        std::cout << "==========================" << std::endl;
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
        mh.recvCode();
        std::cout << "==========================" << std::endl;
}

void MyClient::inputHandler(int choice) 
{
        switch(choice) 
        {
                case static_cast<int>(Protocol::COM_LIST_NG):
                {
                        mh.sendCode(choice);
                        mh.sendCode(static_cast<int>(Protocol::COM_END));
                        int ans = mh.recvCode();
                        if(ans != static_cast<int>(Protocol::ANS_LIST_NG)) {throw new ConnectionClosedException;}
                        int size = mh.recvIntParameter();
                        if(size != 0) 
                        {
                                std::cout << "==============================" << std::endl;
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
                        std::cout << "==============================" << std::endl;
                        if(end != static_cast<int>(Protocol::ANS_END)) {throw new ConnectionClosedException;}
                        break;
                }
                case static_cast<int>(Protocol::COM_CREATE_NG):
                {       
                        mh.sendCode(choice);
                        std::string title;
                        std::cout << "Write newsgroup title" << std::endl;
                        std::getline(std::cin, title);
                        mh.sendStringParameter(title);
                        mh.sendCode(static_cast<int>(Protocol::COM_END));
                        int ans = mh.recvCode();
                        if(ans != static_cast<int>(Protocol::ANS_CREATE_NG)) {throw new ConnectionClosedException;}
                        int ack = mh.recvCode();
                        if(ack == static_cast<int>(Protocol::ANS_NAK)) 
                        {
                                mh.recvCode();
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
                        // List all newsgroup before deleting one
                        inputHandler(1);

                        mh.sendCode(choice);
                        int id;
                        std::cout << "Write newsgroup id to remove:" << std::endl;
                        std::cin >> id;
                        mh.sendIntParameter(id);
                        mh.sendCode(static_cast<int>(Protocol::COM_END));
                        int ans = mh.recvCode();
                        if(ans != static_cast<int>(Protocol::ANS_DELETE_NG)) {throw new ConnectionClosedException;}
                        int ack = mh.recvCode();
                        if(ack == static_cast<int>(Protocol::ANS_ACK)) 
                        {
                                std::cout << "Newsgroup with ID " << id << " removed" << std::endl;
                                std::cout << "Existing newsgroups:" << std::endl;
                        }
                        else 
                        {
                                int err = mh.recvCode();
                                if (err != static_cast<int>(Protocol::ERR_NG_DOES_NOT_EXIST)) {throw new ConnectionClosedException;}
                                std::cout << "Failed to remove newsgroup" << std::endl;
                        }
                        int end = mh.recvCode();
                        if(end != static_cast<int>(Protocol::ANS_END)) {throw new ConnectionClosedException;}
                        inputHandler(1); // List all newsgroups after deletion to give feedback.
                        break;
                }
                case static_cast<int>(Protocol::COM_LIST_ART):
                {       
                        inputHandler(1);

                        int id;
                        std::cout << "Choose newsgroup id:" << std::endl;
                        std::cin >> id;
                        listArticles(id);
                        break;
                }
                case static_cast<int>(Protocol::COM_CREATE_ART):
                {
                        inputHandler(1);

                        mh.sendCode(choice);
                        int id;
                        std::cout << "Choose newsgroup id:" << std::endl;
                        std::cin >> id;
                        std::cin.ignore();
                        std::cout << "Write title:" << std::endl;
                        std::string title;
                        std::getline(std::cin, title);
                        std::cout << "Write author:" << std::endl;
                        std::string author;
                        std::getline(std::cin, author);
                        std::cout << "Write article text:" << std::endl;
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
                        // List all newsgroup and then list all articles in that newsgroup
                        inputHandler(1);

                        int groupID;
                        std::cout << "Choose newsgroup id:" << std::endl;
                        std::cin >> groupID;
                        listArticles(groupID);

                        mh.sendCode(choice);
                        mh.sendIntParameter(groupID);
                        int articleID;
                        std::cout << "Choose article id:" << std::endl;
                        std::cin >> articleID;
                        mh.sendIntParameter(articleID);
                        mh.sendCode(static_cast<int>(Protocol::COM_END));
                        int ans = mh.recvCode();
                        if(ans != static_cast<int>(Protocol::ANS_DELETE_ART)) {throw new ConnectionClosedException;}
                        int ack = mh.recvCode();
                        if(ack == static_cast<int>(Protocol::ANS_ACK)) 
                        {
                                std::cout << "Article with ID " << articleID << " removed" << std::endl;
                                std::cout << "Existing articles:" << std::endl;
                        } else {
                                int err = mh.recvCode();
                                if(err == static_cast<int>(Protocol::ERR_NG_DOES_NOT_EXIST)) {
                                     std::cout << "Failed to find newsgroup" << std::endl;   
                                } else {
                                        std::cout << "Failed to find article" << std::endl;   
                                }
                        }
                        int end = mh.recvCode();
                        listArticles(articleID);
                        
                        if(end != static_cast<int>(Protocol::ANS_END)) {throw new ConnectionClosedException;}
                        break;
                }
                case static_cast<int>(Protocol::COM_GET_ART):
                {
                        inputHandler(1);
                        // TODO add return statement to inputhandler to check if it is empty
                        
                        int groupID;
                        std::cout << "Choose newsgroup id:" << std::endl;
                        std::cin >> groupID;
                        listArticles(groupID);

                        mh.sendCode(choice);
                        mh.sendIntParameter(groupID);
                        int articleID;
                        std::cout << "Choose article id:" << std::endl;
                        std::cin >> articleID;
                        mh.sendIntParameter(articleID);
                        mh.sendCode(static_cast<int>(Protocol::COM_END));
                        int ans = mh.recvCode();
                        if(ans != static_cast<int>(Protocol::ANS_GET_ART)) {throw new ConnectionClosedException;}
                        int ack = mh.recvCode();
                        if(ack == static_cast<int>(Protocol::ANS_ACK)) 
                        {
                                std::cout << "Title: " << mh.recvStringParameter() << std::endl;
                                std::cout << "Author: " << mh.recvStringParameter() << std::endl;
                                std::cout << "Text: " << std::endl;
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
                        throw new ConnectionClosedException();
                        break;
                }
        }
}

void MyClient::printMenu() 
{
        std::cout << "---------------------" << std::endl;
        std::cout << "1: List newsgroups" << std::endl;
        std::cout << "2: Create newsgroup" << std::endl;
        std::cout << "3: Delete newsgroup" << std::endl;
        std::cout << "4: List articles" << std::endl;
        std::cout << "5: Create article" << std::endl;
        std::cout << "6: Delete article" << std::endl;
        std::cout << "7: Get article" << std::endl;
        std::cout << "Type a number(1-7): " << std::endl;
}

int MyClient::run()
{
        int nbr;
        //std::cout << "Type a number: " << std::endl;
        
        while(true) {
                try {
                        printMenu();
                        bool num = true;
                        std::cin >> nbr;
                        std::cin.ignore();

                        while(true) 
                        {
                                if(std::cin.fail()) 
                                {
                                        num = false;
                                        std::cin.clear();
                                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
                                } else {
                                        break;
                                }
                        }
                        if(nbr < 1 || nbr > 7) 
                        {
                                num = false;
                        }
                        if(num) 
                        {
                                inputHandler(nbr);
                        } else {
                                std::cout << "Wrong input, write a number between 1 and 7." << std::endl;
                        }
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
