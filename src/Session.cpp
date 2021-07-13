#include "Session.h"
#include "json.hpp"
#include "Watchable.h"
#include "User.h"
#include "Action.h"
#include <fstream>
using namespace std;
using namespace nlohmann;

Session::Session(const std::string &configFilePath) : content{}, actionsLog{}, userMap{}, activeUser(nullptr)
{
    parseFile(configFilePath);
}
Session::Session(const Session& other):content{},actionsLog{},userMap{},activeUser(nullptr)
{
    for (Watchable* watchable:other.content)
    {
        this->content.push_back(watchable->clone());
    }
    for (BaseAction* action:other.actionsLog)
    {
        this->actionsLog.push_back(action->clone());
    }
    for (auto it:other.userMap)
    {
        this->userMap.insert({it.first,it.second->clone()});
        for (Watchable* watchable:other.userMap.at(it.first)->get_history()) //for each watchable in the original user history
        {
            this->userMap.at(it.first)->pushToHistory(this->content.at(watchable->getId()-1));//push the matching watchable from my session content vector
        }
    }
    this->activeUser=this->userMap.at(other.activeUser->getName());
}
Session::~Session()
{
    for (auto& it:userMap) //delete all users in users map
    {
        User* user=it.second;
        delete user;
    }
    for (BaseAction* action :actionsLog) //delete all actions
    {
        delete action;
    }
    for (Watchable* watchable:content) // delete all watchable
    {
        delete watchable;
    }
}
Session::Session(Session &&other): content(other.content),actionsLog(other.actionsLog),userMap(other.userMap),activeUser(other.activeUser) {
    other.content = {};
    other.actionsLog = {};
    other.userMap = {};
    other.activeUser = nullptr;
}
Session &Session::operator=(const Session &other)
{
    if(this != &other) {
        clear();
        for (Watchable *watchable:other.content) {
            this->content.push_back(watchable->clone());
        }
        for (BaseAction *action:other.actionsLog) {
            this->actionsLog.push_back(action->clone());
        }
        for (auto it:other.userMap) {
            this->userMap.insert({it.first, it.second->clone()});
            for (Watchable *watchable:other.userMap.at(
                    it.first)->get_history()) //for each watchable in the original user history
            {
                this->userMap.at(it.first)->pushToHistory(this->content.at(
                        watchable->getId() - 1));//push the matching watchable from my session content vector
            }
        }
        this->activeUser = this->userMap.at(other.activeUser->getName());
    }
    return *this;
}
Session &Session::operator=(Session &&other) {
    if(this != &other){
        clear();
        content = other.content;
        actionsLog = other.actionsLog;
        userMap = other.userMap;
        activeUser = other.activeUser;
        other.content = {};
        other.actionsLog = {};
        other.userMap = {};
        other.activeUser = nullptr;
    }
    return *this;
}
void Session::clear() {
    for (auto& it:userMap) //delete all users in users map
    {
        User *user = it.second;
        delete user;
    }
    for (BaseAction* action :actionsLog) //delete all actions
    {
        delete action;
    }
    for (Watchable* watchable:content) // delete all watchable
    {
        delete watchable;
    }
    content={};
    actionsLog={};
    userMap={};
}
void Session::parseFile(const std::string &configFilePath)
{
    long id=1;
    ifstream i(configFilePath);
    json jsonFile;
    i>>jsonFile;
    for (size_t i=0;i<jsonFile["movies"].size();i++,id++)
    {
        vector<string> tags;
        string name=jsonFile["movies"][i]["name"];
        int length=jsonFile["movies"][i]["length"];
        for (size_t j=0;j<jsonFile["movies"][i]["tags"].size();j++)
            tags.push_back(jsonFile["movies"][i]["tags"][j].get<string>());
        Watchable* movie=new Movie(id,name,length,tags);
        content.push_back(movie);
    }
    for (size_t i=0;i<jsonFile["tv_series"].size();i++)
    {
        vector<string> tags;
        string name=jsonFile["tv_series"][i]["name"];
        int length=jsonFile["tv_series"][i]["episode_length"];
        for (size_t j=0;j<jsonFile["tv_series"][i]["tags"].size();j++)
            tags.push_back(jsonFile["tv_series"][i]["tags"][j].get<string>());
        for (unsigned int seasonNum=0;seasonNum<jsonFile["tv_series"][i]["seasons"].size();seasonNum++)
        {
            for (unsigned int episodeNum = 0;episodeNum < jsonFile["tv_series"][i]["seasons"][seasonNum].get<unsigned int>(); episodeNum++, id++)
            {
                Watchable *episode = new Episode(id, name, length, seasonNum+1, episodeNum+1, tags);
                content.push_back(episode);
                if ( (seasonNum==jsonFile["tv_series"][i]["seasons"].size()-1) & (episodeNum==jsonFile["tv_series"][i]["seasons"][seasonNum].get<unsigned int>()-1)) //if last episode in last season nextEpisodeId=-1
                    episode->setNextEpisodeId(-1);
                else episode->setNextEpisodeId(id+1);
            }
        }
    }

}
std::vector<BaseAction*>& Session::getActionsLog()
{
    return this->actionsLog;
}
User*& Session::getActiveUser()
{
    return this->activeUser;
}
std::vector<Watchable*>& Session::getContent()
{
    return this->content;
}
std::unordered_map<std::string,User*>& Session::getUserMap()
{
    return this->userMap;
}
void Session::start()
{
    bool exit = false;
    cout<<"SPLFLIX is now on!"<<endl;
    if (userMap.find("default")==userMap.end())
    {
        BaseAction *CreateDefaultUser = new CreateUser("default", "len");
        CreateDefaultUser->act(*this);
        this->ChangeActiveUser(getUserMap().at("default"));
        this->getActionsLog().push_back(CreateDefaultUser);
    }
    while(!exit)
    {
        BaseAction* currentAction;
        std::vector<std::string> stringByWords={};
        string input ="";
        std::getline(std::cin,input);
        splitStringbyWords(input,stringByWords);
        if (stringByWords[0].compare("createuser")==0)
        {
            currentAction=new CreateUser(stringByWords[1],stringByWords[2]);
            currentAction->act(*this);
            this->getActionsLog().push_back(currentAction);
        }
        if (stringByWords[0].compare("changeuser")==0)
        {
            currentAction=new class ChangeActiveUser(stringByWords[1]);
            currentAction->act(*this);
            this->getActionsLog().push_back(currentAction);
        }
        if (stringByWords[0].compare("deleteuser")==0)
        {
            currentAction=new DeleteUser(stringByWords[1]);
            currentAction->act(*this);
            this->getActionsLog().push_back(currentAction);
        }
        if (stringByWords[0].compare("dupuser")==0)
        {
            currentAction=new DuplicateUser(stringByWords[1],stringByWords[2]);
            currentAction->act(*this);
            this->getActionsLog().push_back(currentAction);
        }

        if (stringByWords[0].compare("content")==0)
        {
            currentAction=new PrintContentList();
            currentAction->act(*this);
            this->getActionsLog().push_back(currentAction);
        }
        if (stringByWords[0].compare("watchlist")==0)
        {
            currentAction=new PrintWatchHistory();
            currentAction->act(*this);
            this->getActionsLog().push_back(currentAction);
        }
        if (stringByWords[0].compare("watch")==0)
        {
            long chosenId=std::stol(stringByWords[1]);
            WatchAction(currentAction,chosenId);
        }
        if (stringByWords[0].compare("log")==0)
        {
            currentAction=new PrintActionsLog();
            currentAction->act(*this);
            this->getActionsLog().push_back(currentAction);
        }
        if (stringByWords[0].compare("exit")==0)
        {
            currentAction=new Exit();
            currentAction->act(*this);
            this->getActionsLog().push_back(currentAction);
            exit=true;
        }
    }
}
void Session::ChangeActiveUser(User *newUser)
{
    this->activeUser= newUser;
}
void Session::WatchAction(BaseAction*& currentAction,long chosenId)
{
    currentAction=new Watch(chosenId);
    currentAction->act(*this);
    this->getActionsLog().push_back(currentAction);
    Watchable* Watched=getContent().at(chosenId-1);
    getActiveUser()->pushToHistory(Watched); //add the watched content to watch history of current user
    Watchable* recommendation=getActiveUser()->getRecommendation(*this);
    if (recommendation!= nullptr)
    {
        cout << "We recommend watching " + recommendation->toString() + ", continue watching? [y/n]"<<endl;
        char yesOrNo;
        cin >> yesOrNo;
        if (yesOrNo == 'y') {
            WatchAction(currentAction, recommendation->getId());
        }
    }
    return;

}
void Session::splitStringbyWords(string sentence, vector<std::string> &stringByWords)
{
    string word="";
    for (auto letter:sentence)
    {
        if (letter==' ')
        {
            stringByWords.push_back(word);
            word="";
        }
        else
        {
            word=word+letter;
        }
    }
    stringByWords.push_back(word);
}








