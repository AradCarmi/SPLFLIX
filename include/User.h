#ifndef USER_H_
#define USER_H_

#include <vector>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include "Watchable.h"

class Watchable;
class Session;

class User{
public:
    //Constructor
    User(const std::string& name);
    //Destructor
    virtual ~User();
    User(const User& user);
    User(User &&other);
    User& operator=(const User &other);
    User& operator=(User &&other);
    virtual Watchable* getRecommendation(Session& s) = 0;
    std::string getName() const;
    std::vector<Watchable*> get_history() const;
    // return last content been seen
    Watchable* getLastContent();
    //return ref for next recommended movie
    virtual Watchable *searchForNextWatchable(Session &s)=0;
    //return if this content seen already
    bool seen(Watchable &content);
    //return next episode in the season
    Watchable* getNextRecommendationForEpisode(long nextEpisodeId, Session &s);
    void pushToHistory(Watchable *watchable);
    virtual User* clone() const=0;
    void ChangeName(std::string newName);
protected:
    std::vector<Watchable*> history;
private:
    std::string name;

};
class LengthRecommenderUser : public User {
public:
    LengthRecommenderUser(const std::string& name);
    LengthRecommenderUser(const LengthRecommenderUser& user);
    LengthRecommenderUser(LengthRecommenderUser &&other);
    virtual Watchable* getRecommendation(Session& s);
    //return the avarage length within the history
    int searchAvgLength();
    //return next movie/episode by length
    virtual Watchable* searchForNextWatchable( Session &s);
    virtual LengthRecommenderUser* clone() const;

private:
};
class RerunRecommenderUser : public User {
public:
    RerunRecommenderUser(const RerunRecommenderUser& user);
    RerunRecommenderUser(const std::string& name);
    RerunRecommenderUser(RerunRecommenderUser &&other);
    RerunRecommenderUser& operator=(const RerunRecommenderUser &other);
    RerunRecommenderUser& operator=(RerunRecommenderUser &&other);
    virtual Watchable* getRecommendation(Session& s);
    virtual Watchable* searchForNextWatchable(Session &s);
    virtual RerunRecommenderUser* clone() const;
private:
    //index of last recommended content
    int lastRecommendedContent;
};
class GenreRecommenderUser : public User {
public:
    GenreRecommenderUser(const GenreRecommenderUser& user);
    GenreRecommenderUser(const std::string& name);
    GenreRecommenderUser(GenreRecommenderUser &&other);
    GenreRecommenderUser&operator=(const GenreRecommenderUser &other);
    GenreRecommenderUser&operator=(GenreRecommenderUser &&other);
    virtual Watchable* getRecommendation(Session& s);
    virtual Watchable* searchForNextWatchable(Session &s);
    void addTag(Watchable &watchable);
    bool find(std::string str , std::vector<std::string> vector);
    virtual GenreRecommenderUser* clone() const;
private:
    std::unordered_map<std::string,int> tagsHistogram;
};

#endif