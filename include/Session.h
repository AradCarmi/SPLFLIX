#ifndef SESSION_H_
#define SESSION_H_

#include <vector>
#include <unordered_map>
#include <string>
#include "Action.h"

class User;
class Watchable;

class Session{
public:
    Session(const std::string &configFilePath);
    ~Session();
    void start();
    //mine
    Session(const Session& other);
    Session& operator=(const Session& other);
    Session(Session &&other);
    Session& operator=(Session &&other);
    void clear();
    std::vector<Watchable*>& getContent();
    std::vector<BaseAction*>& getActionsLog();
    std::unordered_map<std::string,User*>& getUserMap();
    User*& getActiveUser();
    void parseFile(const std::string &configFilePath);
    void ChangeActiveUser(User* newUser);
    void splitStringbyWords(string sentence,vector<std::string>& stringByWords);

    //

private:
    std::vector<Watchable*> content;
    std::vector<BaseAction*> actionsLog;
    std::unordered_map<std::string,User*> userMap;
    User* activeUser;
    //mine
    void WatchAction(BaseAction*& currentAction,long id);
};
#endif
