#include "Action.h"
#include "Session.h"
#include "User.h"
//***********************************BaseAction******************************************

BaseAction::BaseAction() : errorMsg(""),status(PENDING)
{
}
ActionStatus BaseAction::getStatus() const
{
    return this->status;
}
void BaseAction::complete()
{
    this->status=COMPLETED;
}
void BaseAction::error(const std::string &errorMsg)
{
    cout<<"ERROR "+errorMsg<<endl;
    this->errorMsg=errorMsg;
    status=ERROR;
}
std::string BaseAction::getErrorMsg() const
{
    return this->errorMsg;
}
string BaseAction::getStatusString() const
{
    switch(status)
    {
        case PENDING: return "PENDING";
        case COMPLETED: return "COMPLETED";
        case ERROR: return "ERROR:"+errorMsg;
    }
    return "";
}
BaseAction::~BaseAction()
{
}
BaseAction::BaseAction(const BaseAction &action) :errorMsg(action.errorMsg),status(action.status)
{
}

//************************************CreateUser*******************************************

CreateUser::CreateUser(string name, string algo) : name(name), algo(algo)
{

}
CreateUser::CreateUser(const CreateUser &action) :BaseAction(action),name(action.name),algo(action.algo){}
void CreateUser::act(Session &sess)
{
    User* newUser;
    unordered_map<std::string,User*> usersMap=sess.getUserMap();
    if (usersMap.find(name)!= usersMap.end() )
    {
        this->error("there is already user with that name");
        return;
    }
    if (this->algo.compare("len")==0) {
        newUser = new LengthRecommenderUser(name);
    } else if (this->algo.compare("rer")==0) {
        newUser = new RerunRecommenderUser(name);
    } else if (this->algo.compare("gen")==0) {
        newUser = new GenreRecommenderUser(name);
    } else{
        this->error("3 letter code is invalid");
        return;
    }

    sess.getUserMap().insert({name,newUser});
    complete();
}
std::string CreateUser::toString() const
{
    return "CreateUser "+getStatusString();
}
CreateUser::~CreateUser()
{

}
CreateUser* CreateUser::clone() const
{
    return new CreateUser(*this);
}

//*************************************ChangeActiveUser************************************

ChangeActiveUser::ChangeActiveUser(const ChangeActiveUser &action) :BaseAction(action),name(action.name){}
ChangeActiveUser::ChangeActiveUser(string name) : name(name)
{

}
void ChangeActiveUser::act(Session &sess)
{
    unordered_map<std::string,User*>& usersMap=sess.getUserMap();
    if (usersMap.find(name)==usersMap.end())
    {
        this->error("user doesn't exsist");
        return;
    }
    User* userToChange=usersMap.at(name);
    sess.ChangeActiveUser(userToChange);
    complete();
}
std::string ChangeActiveUser::toString() const
{
    return "ChangeActiveUser "+getStatusString();
}
ChangeActiveUser::~ChangeActiveUser() {

}
ChangeActiveUser* ChangeActiveUser::clone() const
{
    return new ChangeActiveUser(*this);
}

//**************************************DeleteUser******************************************

DeleteUser::DeleteUser(const DeleteUser &action) :BaseAction(action),name(action.name){}
DeleteUser::DeleteUser(string name) :name(name)
{
}
void DeleteUser::act(Session &sess)
{
    unordered_map<std::string,User*>& usersMap=sess.getUserMap();
    if (usersMap.find(name)==usersMap.end())
    {
        this->error("user doesn't exsist");
        return;
    }
    User* userToDelete=usersMap.at(name);
    delete userToDelete;
    usersMap.erase(name);
    complete();
}
std::string DeleteUser::toString() const
{
    return "DeleteUser "+getStatusString();
}
DeleteUser::~DeleteUser() {

}
DeleteUser* DeleteUser::clone() const
{
    return new DeleteUser(*this);
}

//*************************************DuplicateUser***************************************

DuplicateUser::DuplicateUser(const DuplicateUser &action) :BaseAction(action),originalUserName(action.originalUserName),newUserName(action.newUserName){}
DuplicateUser::DuplicateUser(string originalUserName, string newUserName) :originalUserName(originalUserName),newUserName(newUserName)
{

}
void DuplicateUser::act(Session &sess)
{
    unordered_map<std::string,User*>& usersMap=sess.getUserMap();
    if (usersMap.find(originalUserName)==usersMap.end())
    {
        this->error("original user doesn't exsist");
        return;
    }
    if(usersMap.find(newUserName)!=usersMap.end())
    {
        this->error("new user name already taken");
        return;
    }
    usersMap.insert({newUserName,usersMap.at(originalUserName)->clone()});
    usersMap.at(newUserName)->ChangeName(newUserName);
    complete();

}
std::string DuplicateUser::toString() const
{
    return "DuplicateUser "+getStatusString();
}
DuplicateUser::~DuplicateUser() {

}
DuplicateUser* DuplicateUser::clone() const
{
    return new DuplicateUser(*this);
}

//*************************************PrintContentList*************************************

PrintContentList::PrintContentList(const PrintContentList &action) :BaseAction(action) {}
void PrintContentList::act(Session &sess)
{

    for (Watchable* currentWatchable : sess.getContent())
    {
        string print=to_string(currentWatchable->getId()) + ". " + currentWatchable->toString() + " " + to_string(currentWatchable->getLength())+ " minutes" ;
        for (unsigned int i=0;i<currentWatchable->getTags().size();i++)
        {
            if (i==0) print+=" [";
            print+=currentWatchable->getTags()[i];
            if (i!= currentWatchable->getTags().size()-1) print+=", ";
            else print+="]";
        }
        cout<<print<<endl;
        complete();
    }
}
std::string PrintContentList::toString() const
{
    return "PrintContentList "+getStatusString();
}
PrintContentList::~PrintContentList() {

}
PrintContentList::PrintContentList() {}
PrintContentList* PrintContentList::clone() const
{
    return new PrintContentList(*this);
}

//*************************************PrintWatchHistory************************************

PrintWatchHistory::PrintWatchHistory(const PrintWatchHistory &action):BaseAction(action) {}
void PrintWatchHistory::act(Session &sess)
{
    std::vector<Watchable*> history=sess.getActiveUser()->get_history();
    cout<< "Watch history for " + sess.getActiveUser()->getName()<<endl;
    int i=1;
    for (Watchable*  currentWatchable : history)
    {
        cout<<to_string(i)+"."+currentWatchable->toString()<<endl;
        i++;
    }
    complete();
}
std::string PrintWatchHistory::toString() const
{
    return "PrintWatchHistory "+getStatusString();
}
PrintWatchHistory::~PrintWatchHistory() {

}
PrintWatchHistory::PrintWatchHistory() {}
PrintWatchHistory* PrintWatchHistory::clone() const
{
    return new PrintWatchHistory(*this);
}

//*************************************Watch*************************************************

Watch::Watch(const Watch &action) :BaseAction(action),id(action.id){}
Watch::Watch(long id): id(id) {
}
long Watch::getId()
{
    return this->id;
}
void Watch::act(Session &sess)
{
    std::vector<Watchable*> content=sess.getContent();
    cout<<"Watching "+ content.at(id-1)->toString()<<endl;
    complete();
}
std::string Watch::toString() const
{
    return "Watch "+getStatusString();
}
Watch::~Watch() {

}
Watch* Watch::clone() const
{
    return new Watch(*this);
}

//****************************************PrintActionLog****************************************

PrintActionsLog::PrintActionsLog(const PrintActionsLog &action):BaseAction(action) {}
void PrintActionsLog::act(Session &sess)
{
    std::vector<BaseAction*> actionLog = sess.getActionsLog();
    for(vector<BaseAction*>::reverse_iterator it = actionLog.rbegin(); it != actionLog.rend();++it)
    {
        cout<<(*it)->toString()<<endl;
    }
    complete();
}
std::string PrintActionsLog::toString() const
{
    return "PrintActionLog "+getStatusString();
}
PrintActionsLog::~PrintActionsLog() {

}
PrintActionsLog::PrintActionsLog() {}
PrintActionsLog* PrintActionsLog::clone() const
{
    return new PrintActionsLog(*this);
}
//*****************************************Exit***********************************************

Exit::Exit(const Exit &action) :BaseAction(action){}
void Exit::act(Session &sess)
{
    complete();
}
std::string Exit::toString() const
{
    return "Exit"+getStatusString();
}
Exit::~Exit()
{

}
Exit::Exit() {}
Exit* Exit::clone() const
{
    return new Exit(*this);
}
