#ifndef ACTION_H_
#define ACTION_H_

#include <string>
#include <iostream>
using namespace std;
class Session;

enum ActionStatus{
	PENDING, COMPLETED, ERROR
};


class BaseAction{
public:
	BaseAction();
	ActionStatus getStatus() const;
	virtual void act(Session& sess)=0;
	virtual std::string toString() const=0;
	//mine
	string getStatusString() const;
	virtual ~BaseAction() ;
    virtual BaseAction* clone() const=0;
    BaseAction(const BaseAction& action);
    //
protected:
	void complete();
	void error(const std::string& errorMsg);
	std::string getErrorMsg() const;
private:
	std::string errorMsg;
	ActionStatus status;
};

class CreateUser  : public BaseAction {
public:
	virtual void act(Session& sess);
	virtual std::string toString() const;
	//mine
	CreateUser(const CreateUser& action);
	virtual CreateUser* clone() const;
	virtual ~CreateUser();
	CreateUser(string name,string algo);
	string name;
	string algo;
};

class ChangeActiveUser : public BaseAction {
public:
	virtual void act(Session& sess);
	virtual std::string toString() const;
	//mine
    ChangeActiveUser(const ChangeActiveUser& action);
	virtual ChangeActiveUser* clone() const;
	virtual ~ChangeActiveUser();
	ChangeActiveUser(string name);
	string name;
};

class DeleteUser : public BaseAction {
public:
	virtual void act(Session & sess);
	virtual std::string toString() const;
	//mine
    DeleteUser(const DeleteUser& action);
    virtual DeleteUser* clone() const;
	virtual ~DeleteUser();
	DeleteUser(string name);
	string name;
};

class DuplicateUser : public BaseAction {
public:
	virtual void act(Session & sess);
	virtual std::string toString() const;
	//mine
    DuplicateUser(const DuplicateUser& action);
	virtual DuplicateUser* clone() const;
	virtual ~DuplicateUser();
	DuplicateUser(string originalUserName,string newUserName);
	string originalUserName;
	string newUserName;
};

class PrintContentList : public BaseAction {
public:
	virtual void act (Session& sess);
	virtual std::string toString() const;
	virtual ~PrintContentList();
	//mine
    PrintContentList(const PrintContentList& action);
    virtual PrintContentList* clone() const;
	PrintContentList();
};

class PrintWatchHistory : public BaseAction {
public:
    virtual ~PrintWatchHistory();
	virtual void act (Session& sess);
	virtual std::string toString() const;
	//mine
    PrintWatchHistory(const PrintWatchHistory& action);
    virtual PrintWatchHistory* clone() const;
	PrintWatchHistory();
};


class Watch : public BaseAction {
public:
	virtual void act(Session& sess);
	virtual std::string toString() const;
	//mine
    Watch(const Watch& action);
    virtual Watch* clone() const;
	virtual ~Watch();
	Watch(long id);
	long getId();
private:
    long id;
};


class PrintActionsLog : public BaseAction {
public:
    virtual ~PrintActionsLog();
	virtual void act(Session& sess);
	virtual std::string toString() const;
	//mine
    PrintActionsLog(const PrintActionsLog& action);
	virtual PrintActionsLog* clone() const;
	PrintActionsLog();
};

class Exit : public BaseAction {
public:
    virtual ~Exit();
    virtual void act(Session& sess);
	virtual std::string toString() const;
	//mine
    Exit(const Exit& action);
    virtual Exit* clone() const;
	Exit();
};
#endif
