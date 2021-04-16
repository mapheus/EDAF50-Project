#include "storagememory.h"

#include <stdlib.h>
#include <time.h>  

StorageMemory::StorageMemory()
{
    srand(time(NULL));
}

bool StorageMemory::CreateNewsGroup(const std::string& title)
{
    auto it = find_if(m_NewsGroups.begin(), m_NewsGroups.end(), [&title](std::shared_ptr<NewsGroup>& ng) {
            return title == ng->getTitle();
    });
    if(it == m_NewsGroups.end())
    {
        m_NewsGroups.emplace_back(std::make_shared<NewsGroup>(title, rand()%100000));
        return true;
    }
    else
    {
        return false;
    }
}

std::vector<std::shared_ptr<NewsGroup>> StorageMemory::ListNewsGroups()
{
    return m_NewsGroups;
}

std::shared_ptr<NewsGroup> StorageMemory::GetNewsGroup(int id)
{
    auto it = find_if(m_NewsGroups.begin(), m_NewsGroups.end(), [&id](std::shared_ptr<NewsGroup>& ng) {
            return id == ng->getId();
    });
    if(it != m_NewsGroups.end()) {
        return *it;
    } else {
        return nullptr;
    }
}

bool StorageMemory::DeleteNewsGroup(int id)
{
    auto it = find_if(m_NewsGroups.begin(), m_NewsGroups.end(), [&id](std::shared_ptr<NewsGroup>& ng) {
            return id == ng->getId();
    });
    if(it != m_NewsGroups.end())
    {
        m_NewsGroups.erase(it);
        return true;
    }
    else
    {
        return false;
    }
        
}

bool StorageMemory::CreateArticle(int newsgroup_id, const std::string& title,const std::string& author,const std::string& text)
{
    std::shared_ptr<NewsGroup> ng = GetNewsGroup(newsgroup_id);
    if(ng != nullptr)
    {
        ng->addArticle(std::make_shared<Article>(newsgroup_id, rand()%100000, title, author, text));
        return true;
    }
    else 
    {
        return false;
    }
}

bool StorageMemory::DeleteArticle(int groupID, int id)
{
    GetNewsGroup(groupID)->deleteArticle(id);
    return true;
}