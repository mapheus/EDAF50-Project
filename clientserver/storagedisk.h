#ifndef STORAGEDISK_H
#define STORAGEDISK_H

#include "storage.h"

#include <fstream>

class StorageDisk : public Storage
{
public:
    virtual bool CreateNewsGroup(const std::string& title) override;
    virtual std::vector<std::shared_ptr<NewsGroup>> ListNewsGroups() override;
    virtual std::shared_ptr<NewsGroup> GetNewsGroup(int id) override;
    virtual bool DeleteNewsGroup(int id) override;
    virtual bool DeleteArticle(int groupID, int id) override;

    virtual bool CreateArticle(int newsgroup_id, const std::string& title,const std::string& author,const std::string& text) override;
    std::vector<std::shared_ptr<Article>> GetArticles(int newsgroup_id);
private:
    bool DeleteArticle(int id);
private:
    std::ifstream m_In;
    std::ofstream m_Out;
};

#endif