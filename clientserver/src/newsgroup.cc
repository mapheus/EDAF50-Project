#include "newsgroup.h"
#include <iostream>

bool NewsGroup::deleteArticle(int id) 
{ 
    auto it = std::find_if(articles.begin(), articles.end(), [&id](std::shared_ptr<Article>& a) {
        return a->getId() == id;
    });
    if(it != articles.end())
    {
        articles.erase(it);
        return true;
    }
    else
    {
        return false;
    }
}

    
const std::shared_ptr<Article> NewsGroup::getArticle(int id) 
{
    for(auto& a : articles) {
        if(a->getId() == id) {
            return a;
        }
    }
    return nullptr;
}