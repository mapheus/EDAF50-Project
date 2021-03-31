#ifndef ARTICLE
#define ARTICLE

#include <string>
#include <cstdlib>
#include <random>
#include "article.h"
#include <vector>

using std::string;
using std::vector;

class Article {
public:
    Article(string ti, string au, string te) : id{rand()%100}, title{ti}, author{au}, text{te} {}
    const int& getId() {return id;}
    const string& getTitle() {return title;}
    const string& getAuthor() {return author;}
    const string& getText() {return text;}


private:
    int id; // TODO: not random
    string title; // TODO: Unique in newsgroup
    string author;
    string text;
    
};




#endif 