#include "newsgroup.h"
#include <iostream>


void NewsGroup::deleteArticle(int id) 
{ 
    auto it = std::find_if(articles.begin(), articles.end(), [&id](std::shared_ptr<Article>& a) {
        return a->getId() == id;
    });
    articles.erase(it);
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

