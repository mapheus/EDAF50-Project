#include "storagedisk.h"
#include <sstream>
#include <iostream>

#include <stdlib.h>

bool StorageDisk::CreateNewsGroup(const std::string& title)
{
    //TODO : Check if name exists

    m_Out.open("test.database", std::ios::app);
    if(m_Out.is_open())
    {
        m_Out << "NewsGroup" << std::endl;
        m_Out << "{" << std::endl;
        m_Out << "id " << rand() % 200 << std::endl;
        m_Out << "title " << title << std::endl;
        m_Out << "}" << std::endl << std::endl;
        m_Out.close();
    }
    return true;
}
std::vector<std::shared_ptr<NewsGroup>> StorageDisk::ListNewsGroups()
{
    std::vector<std::shared_ptr<NewsGroup>> newsgroups;
    m_In.open("test.database");
    if(m_In.is_open())
    {
        int ngid;
        std::string title;
        std::vector<int> articles;
        
        bool readingNewsgroup = false;
        std::string line;
        while(std::getline(m_In, line))
        {
            if(readingNewsgroup)
            {
                std::istringstream iss(line);
                std::string word;
                iss >> word;
                if(word == "id")
                {
                    iss >> word;
                    ngid = std::stoi(word);
                }
                if(word == "title")
                {
                    iss >> word;
                    title = word;
                }

                if(line == "}")
                {
                    readingNewsgroup = false;
                    std::shared_ptr<NewsGroup> ng = std::make_shared<NewsGroup>(title, ngid);
                    newsgroups.push_back(ng);
                }
            }
            if(line == "NewsGroup")
            {
                readingNewsgroup = true;
                std::cout << "READING NEWSGROUP" << std::endl;
            }
        }

    m_In.close();
    }
    return newsgroups;
}
std::shared_ptr<NewsGroup> StorageDisk::GetNewsGroup(int id)
{
    std::shared_ptr<NewsGroup> newsgroup;
    m_In.open("test.database");
    if(m_In.is_open())
    {
        int ngid;
        std::string title;
        std::vector<int> articles;
        bool found = false;
        
        bool readingNewsgroup = false;
        std::string line;
        while(std::getline(m_In, line))
        {
            if(readingNewsgroup)
            {
                std::istringstream iss(line);
                std::string word;
                iss >> word;
                if(word == "id")
                {
                    iss >> word;
                    ngid = std::stoi(word);
                    std::cout << ngid << std::endl;
                    if(ngid==id)
                        found = true;
                }
                if(word == "title")
                {
                    iss >> word;
                    title = word;
                }
                if(word == "article")
                {
                    iss >> word;
                    articles.push_back(std::stoi(word));
                }
                
                if(line == "}")
                {
                    readingNewsgroup = false;
                    if(found)
                    {
                        newsgroup = std::make_shared<NewsGroup>(title, ngid);
                        for(auto& i : articles)
                        {
                            newsgroup->addArticle(GetArticle(ngid, i));
                        }
                        m_In.close();
                        return newsgroup;
                    }
                }
            }
            if(line == "NewsGroup")
            {
                readingNewsgroup = true;
                std::cout << "READING NEWSGROUP" << std::endl;
            }
        }

    m_In.close();
    }
    return nullptr;
}
bool StorageDisk::DeleteNewsGroup(int id)
{ 
    m_In.open("test.database");
    if(m_In.is_open())
    {
        int ngid;
        std::string title;
        std::vector<int> articles;
        bool found = false;
        
        bool readingNewsgroup = false;
        std::string line;
        while(std::getline(m_In, line))
        {

        }

    m_In.close();
    }
    return true;
}

bool StorageDisk::CreateArticle(int newsgroup_id, const std::string& title,const std::string& author,const std::string& text)
{
    if(GetNewsGroup(newsgroup_id) != nullptr)
    {
        m_Out.open("test.database", std::ios::app);
        if(m_Out.is_open())
        {
            m_Out << "Article" << std::endl;
            m_Out << "{" << std::endl;
            m_Out << "newsgroup_id " << newsgroup_id << std::endl;
            m_Out << "id " << rand() % 200 << std::endl;
            m_Out << "title " << title << std::endl;
            m_Out << "author " << author << std::endl;
            m_Out << "text " << text << std::endl;
            m_Out << "}" << std::endl << std::endl;
            m_Out.close();
        }
        return true;
    }
    return false;
}

std::shared_ptr<Article> StorageDisk::GetArticle(int newsgroup_id, int id) 
{
    std::shared_ptr<Article> article;
    m_In.open("test.database");
    if(m_In.is_open())
    {
        int aid;
        int ngid;
        std::string title;
        std::string author;
        std::string text;

        bool found = false;
        
        bool readingArticle = false;
        std::string line;
        while(std::getline(m_In, line))
        {
            if(readingArticle)
            {
                std::istringstream iss(line);
                std::string word;
                iss >> word;
                if(word == "newsgroup_id")
                {
                    iss >> word;
                    ngid = std::stoi(word);
                    if(ngid == newsgroup_id)
                        found = true;
                    else
                        found = false;
                }
                if(word == "id")
                {
                    iss >> word;
                    aid = std::stoi(word);
                    if(aid == id)
                        found = true;
                    else
                        found = false;
                }
                if(word == "title")
                {
                    iss >> word;
                    title = word;
                }
                if(word == "author")
                {
                    iss >> word;
                    author = word;
                }
                if(word == "text")
                {
                    iss >> word;
                    text = word;
                }

                if(line == "}")
                {
                    readingArticle = false;
                    if(found)
                    {
                        article = std::make_shared<Article>(newsgroup_id, aid, title, author, text);
                        m_In.close();
                        return article;
                    }
                }
            }
            if(line == "Article")
            {
                readingArticle = true;
            }
        }

    m_In.close();
    }
    return nullptr;
}