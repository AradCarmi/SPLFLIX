#include "User.h"
#include "Session.h"
class Watchable;
class Session;
//**********************************************USER*******************************************

User::User(const std::string &name):history{},name(name)
{
}
User::User(const User &user) :history{},name(user.name)
{
}
User::~User()
{
}
std::string User::getName() const {
    return this->name;
}
std::vector<Watchable *> User::get_history() const {
    return this->history;
}
Watchable *User::getLastContent(){
    return history.back();
}
bool User::seen(Watchable& content) {
    bool exist = false;
    for(auto it = history.cbegin();it != history.cend() && !exist;it++){
        if(*it == &content )
            exist = true;
    }
    return exist;
}
Watchable *User::getNextRecommendationForEpisode(long nextEpisodeId, Session &s) {
    // -1 in the id means there is no next episode
    if(nextEpisodeId == -1)
        return searchForNextWatchable(s);
    return s.getContent()[nextEpisodeId-1];
}
void User::pushToHistory(Watchable *watchable) {
    history.push_back(watchable);

}
void User::ChangeName(std::string newName)
{
    this->name=newName;
}
User::User(User &&other):history(other.history),name(other.name) {
    other.name = "";
    other.history = {};
}
User &User::operator=(const User &other) {
    if(this != &other){
        name = other.name;
        history = other.history;
    }
    return *this;
}
User &User::operator=(User &&other) {
    if(this != &other){
        name = other.name;
        history = other.history;
        other.name = "";
        other.history = {};
    }
    return *this;
}

//********************************************LengthRecommendedUser*****************************

LengthRecommenderUser::LengthRecommenderUser(const std::string &name) :User(name) { }
LengthRecommenderUser::LengthRecommenderUser(const LengthRecommenderUser &user) :User(user){}
LengthRecommenderUser* LengthRecommenderUser::clone() const
{
    return new LengthRecommenderUser(*this);
}
Watchable* LengthRecommenderUser::getRecommendation(Session &s) {
    return getLastContent()->getNextWatchable(s);
}
int LengthRecommenderUser::searchAvgLength()
{
    int sum = 0;
    int counter = 0;
    Watchable* content;
    for(auto it = history.cbegin();it != history.cend();it++)
    {
        content = *it;
        sum = sum + content->getLength();
        counter++;
    }
    if(counter == 0)
        return 0;
    return (sum/counter);
}
Watchable* LengthRecommenderUser::searchForNextWatchable(Session &s)
{
    Watchable *nextWatchable;
    int avgLength = searchAvgLength();
    Watchable* closestToAvg = nullptr;
    for(auto it = s.getContent().cbegin();it != s.getContent().cend();it++)
    {
        nextWatchable= *it;
        if (closestToAvg== nullptr )
        {
            if (!seen(*nextWatchable))
                closestToAvg = nextWatchable;
        }
        else if(abs(nextWatchable->getLength()-avgLength) < abs(closestToAvg->getLength() - avgLength) && !seen(*nextWatchable))
            closestToAvg = nextWatchable;
    }
    return closestToAvg;
}
LengthRecommenderUser::LengthRecommenderUser(LengthRecommenderUser &&other):User(other){
}


//********************************************RerunRecommendedUser*******************************

RerunRecommenderUser::RerunRecommenderUser(const std::string &name) : User(name), lastRecommendedContent(-1){
}
RerunRecommenderUser::RerunRecommenderUser(const RerunRecommenderUser &user) :User(user),lastRecommendedContent(-1){}
RerunRecommenderUser::RerunRecommenderUser(RerunRecommenderUser &&other):User(other),lastRecommendedContent(other.lastRecommendedContent)
{
    lastRecommendedContent = 0;
}
RerunRecommenderUser* RerunRecommenderUser::clone() const
{
    return new RerunRecommenderUser(*this);
}
RerunRecommenderUser& RerunRecommenderUser::operator=(const RerunRecommenderUser &other) {
    return *this;
}
Watchable* RerunRecommenderUser::getRecommendation(Session &s) {
    return getLastContent()->getNextWatchable(s);
}
Watchable* RerunRecommenderUser::searchForNextWatchable(Session &s)
{
    if (lastRecommendedContent!=-1)
    {
        if (this->get_history().at(lastRecommendedContent)->getId() == this->get_history().back()->getId())
            lastRecommendedContent = lastRecommendedContent + 1;
    }
    else lastRecommendedContent=0;
    return history[(lastRecommendedContent)%history.size()];
}
RerunRecommenderUser &RerunRecommenderUser::operator=(RerunRecommenderUser &&other) {
    if(this != &other){
        lastRecommendedContent = other.lastRecommendedContent;
        other.lastRecommendedContent = 0;
    }
    return *this;
}

//**********************************************GenreRecommendedUser*******************************

GenreRecommenderUser::GenreRecommenderUser(const std::string &name): User(name),tagsHistogram{} {
}
GenreRecommenderUser::GenreRecommenderUser(const GenreRecommenderUser &user) :User(user),tagsHistogram(user.tagsHistogram){
}
GenreRecommenderUser::GenreRecommenderUser(GenreRecommenderUser &&other):User(other),tagsHistogram(other.tagsHistogram)
{
    other.tagsHistogram = {};
}
GenreRecommenderUser& GenreRecommenderUser::operator=(const GenreRecommenderUser &other) {
    tagsHistogram = other.tagsHistogram;
    return *this;
}
GenreRecommenderUser& GenreRecommenderUser::operator=(GenreRecommenderUser &&other) {
    if(this != &other){
        tagsHistogram = other.tagsHistogram;
        other.tagsHistogram = {};
    }
    return *this;
}
GenreRecommenderUser* GenreRecommenderUser::clone() const
{
    return new GenreRecommenderUser(*this);
}
Watchable* GenreRecommenderUser::getRecommendation(Session &s) {
    Watchable *lastContent = getLastContent();
    addTag(*lastContent);
    return lastContent->getNextWatchable(s);
}
Watchable* GenreRecommenderUser::searchForNextWatchable(Session &s) {
    Watchable *recommended = nullptr;
    int max;
    string tag;
    std::vector<string> failTags;
    if (tagsHistogram.empty()) {
        return nullptr;
    }
    while (failTags.size() != tagsHistogram.size()) {
        max = -1;
        tag = "";
        for (auto element : tagsHistogram) {
            if(!find(element.first,failTags))
                if((element.second > max) || ((element.second == max) && (lexicographical_compare(element.first.cbegin(),element.first.cend(),tag.cbegin(),tag.cend())))){
                    tag = element.first;
                    max = element.second;
                }
        }
        for (size_t i = 0; i != s.getContent().size(); ++i) {
            recommended = s.getContent()[i];
            std::vector<string> currentTags = s.getContent()[i]->getTags();
            if (find(tag,currentTags))
                if (!seen(*recommended))
                    return recommended;
        }
        failTags.push_back(tag);
    }
    return nullptr;
}
void GenreRecommenderUser::addTag(Watchable &watchable) {
    for(string element : watchable.getTags()){
        if(tagsHistogram.find(element) == tagsHistogram.end()){
            tagsHistogram.insert({element,1});
        }
        else{
            tagsHistogram.at(element) = tagsHistogram.at(element) + 1;
        }
    }
}
bool GenreRecommenderUser::find(string str , std::vector<string> vector) {
    for(auto element : vector){
        if(element == str)
            return true;
    }
    return false;
}

