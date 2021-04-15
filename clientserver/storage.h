#ifndef STORAGE_H
#define STORAGE_H

#include <string>
#include <vector>
#include <memory>

#include "newsgroup.h"
#include "article.h"
#include "../protocol.h"


enum class StorageType
{
    Memory, Disk
};

class Storage {
public:
    virtual bool CreateNewsGroup(const std::string& title) = 0;
    virtual std::vector<std::shared_ptr<NewsGroup>> ListNewsGroups() = 0;
    virtual std::shared_ptr<NewsGroup> GetNewsGroup(int id) = 0;
    virtual bool DeleteNewsGroup(int id) = 0;
    virtual bool DeleteArticle(int groupID, int id) = 0;

    virtual bool CreateArticle(int newsgroup_id, const std::string& title,const std::string& author,const std::string& text ) = 0;
    virtual std::vector<std::shared_ptr<Article>> GetArticles(int newsgroup_id) = 0;
private:

};

#endif