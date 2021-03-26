#ifndef NEWSGROUP
#define NEWSGROUP

#include <string>
#include <cstdlib>
#include <random>
#include "article.h"
#include <vector>
#include <memory>
#include <algorithm>

using std::string;
using std::vector;

class NewsGroup {
public:
    NewsGroup(string title) : id{rand()}, t{title} {}
    void addArticle(const std::shared_ptr<Article>& a) { articles.push_back(a); }
    const vector<std::shared_ptr<Article>>& getArticles() {return articles;}
    void deleteArticle(int id);
    const std::shared_ptr<Article>& getArticle(int id);
    int getId() {return id;}
    string getTitle() {return t;}

private:
    int id; // TODO unique not random
    string t; // TODO unique
    vector<std::shared_ptr<Article>> articles;
};




#endif 