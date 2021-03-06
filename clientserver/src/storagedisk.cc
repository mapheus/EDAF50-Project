#include "storagedisk.h"
#include <sstream>
#include <iostream>

#include <stdlib.h>
#include <time.h>  

StorageDisk::StorageDisk()
{
    srand(time(NULL));
}

bool StorageDisk::CreateNewsGroup(const std::string& title)
{
    std::vector<std::shared_ptr<NewsGroup>> newsgroups = ListNewsGroups();
    auto it = find_if(newsgroups.begin(), newsgroups.end(), [&title](std::shared_ptr<NewsGroup>& ng) {
            return title == ng->getTitle();
    });
    if(it != newsgroups.end())
    {
        return false;
    }
    else
    {
        m_Out.open("test.database", std::ios::app);
        if(m_Out.is_open())
        {
            m_Out << "NewsGroup" << std::endl;
            m_Out << "{" << std::endl;
            m_Out << "id " << rand() % 100000 << std::endl;
            m_Out << "title " << title << std::endl;
            m_Out << "}" << std::endl << std::endl;
            m_Out.close();
        }
        return true;
    }
}

std::vector<std::shared_ptr<NewsGroup>> StorageDisk::ListNewsGroups()
{
    std::vector<std::shared_ptr<NewsGroup>> newsgroups;
    m_In.open("test.database");
    if(m_In.is_open())
    {
        int ngid = -1;
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
                    title = line.substr(6);
                }

                if(line == "}" || line == "} " || word == "}" || word == "} " )
                {
                    readingNewsgroup = false;
                    std::shared_ptr<NewsGroup> ng = std::make_shared<NewsGroup>(title, ngid);
                    newsgroups.push_back(ng);
                }
            }
            if(line == "NewsGroup")
            {
                readingNewsgroup = true;
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
        int ngid = -1;
        std::string title;
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
                    if(ngid==id)
                    {
                        found = true;
                    }
                }
                if(word == "title")
                {
                    title = line.substr(6);
                }
                
                if(word == "} " || word == "}\n" || line == "}" || line == "} ")
                {
                    readingNewsgroup = false;
                    if(found)
                    {
                        m_In.close();
                        newsgroup = std::make_shared<NewsGroup>(title, ngid);
                        std::vector<std::shared_ptr<Article>> articles = GetArticles(ngid);
                        for(auto& e : articles)
                        {
                            newsgroup->addArticle(e);
                        }
                        return newsgroup;
                    }
                }
            }
            if(line == "NewsGroup")
            {
                readingNewsgroup = true;
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
        int ngid = -1;
        std::string title;
        std::vector<int> articles;
        bool found = false;
        
        std::string line;
        int delIndex = 0;
        while(std::getline(m_In, line))
        {
            std::istringstream iss(line);
            std::string word;
            iss >> word;
            if(word == "id")
            {
                iss >> word;
                ngid = std::stoi(word);
                if(ngid==id)
                {
                    found = true;
                    break;
                }
            }
            ++delIndex;
        }
        m_In.close();
        if(found)
        {
            // Read file again and only add rows that does not include id to remove
            m_In.open("test.database");
            // Create database2
            m_Out.open("test.database2", std::ios::app);
            int index = 0;
            while(std::getline(m_In, line))
            {
                std::istringstream iss(line);
                std::string word;
                iss >> word;
                if(index < delIndex - 2 || index > delIndex + 2) 
                {
                    m_Out << line << std::endl;
                }
                ++index;
            }
            m_In.close();
            m_Out.close();
            remove("test.database");
            rename("test.database2", "test.database");
        }
    }
    bool t = true;
    while(t) 
    {
        t = DeleteArticlesInNewsgroup(id);
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
            m_Out << "id " << rand() % 100000 << std::endl;
            m_Out << "title " << title << std::endl;
            m_Out << "author " << author << std::endl;
            m_Out << "text \n" << text << std::endl;
            m_Out << "}" << std::endl << std::endl;
            m_Out.close();
        }
        return true;
    }
    return false;
}


std::vector<std::shared_ptr<Article>> StorageDisk::GetArticles(int newsgroup_id)
{
    std::vector<std::shared_ptr<Article>> articles;
    m_In.open("test.database");
    if(m_In.is_open())
    {
        int aid = -1;
        int ngid = -1;
        std::string title;
        std::string author;
        std::string text;
        
        bool readingText = false;
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
                }
                if(word == "id")
                {
                    iss >> word;
                    aid = std::stoi(word);
                }
                if(word == "title")
                {
                    title = line.substr(6);
                }
                if(word == "author")
                {
                    author = line.substr(7);
                }

                if(ngid == newsgroup_id && (line == "}" || line == "} " || word == "}\n" || word == "}"))
                {
                    readingText = false;
                    readingArticle = false;
                    text.erase(text.end()-1);
                    articles.push_back(std::make_shared<Article>(newsgroup_id, aid, title, author, text));
                    text = "";
                    ngid = -1;
                }

                if(readingText && readingArticle && !(line == "}" || line == "} " || word == "}\n" || word == "}"))
                {
                    text.append(line);
                    text.append("\n");
                }

                if(word == "text")
                {
                    readingText=true;
                }

            }
            if(line == "Article")
            {
                readingArticle = true;
            }   
        }
        m_In.close();
    }
    return articles;
}

bool StorageDisk::DeleteArticlesInNewsgroup(int newsgroupd_id)
{ 
    bool found = false;
    m_In.open("test.database");
    if(m_In.is_open())
    {
        int ngid;
        std::string title;
        std::vector<int> articles;
        
        std::string line;
        int startIndex = INT16_MAX;
        int lineIndex = 0;
        int lastIndex = 0; 
        while(std::getline(m_In, line))
        {
            std::istringstream iss(line);
            std::string word;
            iss >> word;
            if(word == "newsgroup_id")
            {
                iss >> word;
                ngid = std::stoi(word);
                if(ngid==newsgroupd_id)
                {
                    found = true;
                    startIndex = lineIndex;
                }
            }
            if((word == "} " || word == "}\n" || line == "}" || line == "} ") && found) 
            {
                lastIndex = lineIndex;
                m_In.close();
                break;
            }
            
            ++lineIndex;
        }
        m_In.close();
        if(found) 
        {
            // Read file again and only add rows that does not include id to remove
            m_In.open("test.database");
            // Create database2
            m_Out.open("test.database2", std::ios::app);
            int index = 0;

            while(std::getline(m_In, line))
            {
                std::istringstream iss(line);
                std::string word;
                iss >> word;
                if(index < startIndex - 2 || index > lastIndex) 
                {
                    m_Out << line << std::endl;
                }
                ++index;
            }
            m_In.close();
            m_Out.close();
            remove("test.database");
            rename("test.database2", "test.database");
        }
        
    }
    return found;
}

bool StorageDisk::DeleteArticle(int groupID, int id)
{
    bool foundID = false;
    bool foundNG = false;
    m_In.open("test.database");
    if(m_In.is_open())
    {   
        std::string line;
        int startIndex = INT16_MAX;
        int lineIndex = 0;
        int lastIndex = 0; 
        while(std::getline(m_In, line))
        {
            std::istringstream iss(line);
            std::string word;
            iss >> word;
            if(word == "newsgroup_id")
            {
                iss >> word;
                int ngid = std::stoi(word);
                if(ngid==groupID)
                {
                    foundNG = true;
                    startIndex = lineIndex;
                }
            }
            if(word == "id")
            {
                iss >> word;
                int i = std::stoi(word);
                if(i==id)
                {
                    foundID = true;
                }
            }
            if(word == "}" && foundNG && foundID) 
            {
                lastIndex = lineIndex;
                m_In.close();
                break;
            } else if(word == "}")
            {
                foundNG = false;
                foundID = false;
            }
            
            ++lineIndex;
        }
        if(foundNG && foundID) {
            // Read file again and only add rows that does not include id to remove
            m_In.open("test.database");
            // Create database2
            m_Out.open("test.database2", std::ios::app);
            int index = 0;

            while(std::getline(m_In, line))
            {
                std::istringstream iss(line);
                std::string word;
                iss >> word;
                if(index < startIndex - 2 || index > lastIndex) 
                {
                    m_Out << line << std::endl;
                }
                ++index;
            }
            m_In.close();
            m_Out.close();
            remove("test.database");
            rename("test.database2", "test.database");
        }
    }
    return foundNG && foundID;
}