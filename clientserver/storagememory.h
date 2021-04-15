#ifndef STORAGEMEMORY_H
#define STORAGEMEMORY_H

#include "storage.h"

class StorageMemory : public Storage
{
public:
    virtual bool CreateNewsGroup(const std::string& title) override;
    virtual std::vector<std::shared_ptr<NewsGroup>> ListNewsGroups() override;
    virtual std::shared_ptr<NewsGroup> GetNewsGroup(int id) override;
    virtual bool DeleteNewsGroup(int id) override;

    virtual bool CreateArticle(int newsgroup_id, const std::string& title,const std::string& author,const std::string& text) override;
private:
    std::vector<std::shared_ptr<NewsGroup>> m_NewsGroups;
};

#endif