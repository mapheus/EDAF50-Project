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
    Article(int newsgroup_id, string ti, string au, string te) : m_NewsGroupID{newsgroup_id}, m_ID{rand()%100}, m_Title{ti}, m_Author{au}, m_Text{te} {}
    Article(int newsgroup_id, int id, string ti, string au, string te) : m_NewsGroupID{newsgroup_id}, m_ID{id}, m_Title{ti}, m_Author{au}, m_Text{te} {}
    int getNewsGroupID() { return m_NewsGroupID;}
    int getId() {return m_ID;}
    const string& getTitle() {return m_Title;}
    const string& getAuthor() {return m_Author;}
    const string& getText() {return m_Text;}


private:
    int m_NewsGroupID;
    int m_ID; // TODO: not random
    string m_Title; // TODO: Unique in newsgroup
    string m_Author;
    string m_Text;
    
};




#endif 