#ifndef NEWSGROUP_H
#define NEWSGROUP_H

#include <string>
#include <cstdlib>
#include <random>
#include "article.h"
#include <vector>
#include <memory>
#include <algorithm>

class NewsGroup
{
public:
    NewsGroup(const string& title) : m_ID{rand()%100}, t{title} {}
    NewsGroup(const string& title, int id) : m_ID{id}, t{title} {}
    void addArticle(const std::shared_ptr<Article>& a) { articles.push_back(a); }
    const vector<std::shared_ptr<Article>>& getArticles() {return articles;}
    bool deleteArticle(int id);
    const std::shared_ptr<Article> getArticle(int id);
    int getId() {return m_ID;}
    string getTitle() {return t;}

private:
    vector<std::shared_ptr<Article>> articles;
    int m_ID; // TODO unique not random
    string t; // TODO unique
};

#endif