# targets
all: splflix

#excecutble splflix depands on main,user,action,session,watchable .o
splflix:bin/Main.o bin/User.o bin/Action.o bin/Session.o bin/Watchable.o  
	@echo 'building target splflix'
	g++ -o  bin/splflix bin/Main.o  bin/Session.o   bin/Watchable.o bin/User.o bin/Action.o
	@echo 'finished building target'
	
#main.o depands on main.cpp
bin/Main.o:src/Main.cpp
	@echo 'compiling main'
	g++ -g -Wall -Weffc++ -std=c++11 -c -Iinclude -o bin/Main.o src/Main.cpp
	
#user.o depands on user.cpp
bin/User.o:src/User.cpp
	@echo 'compiling user'
	g++ -g -Wall -Weffc++ -std=c++11 -c -Iinclude -o bin/User.o src/User.cpp
	
#action.o depands on action.cpp
bin/Action.o:src/Action.cpp
	@echo 'compiling action'
	g++ -g -Wall -Weffc++ -std=c++11 -c -Iinclude -o bin/Action.o src/Action.cpp

#sesion.o depands on session .cpp
bin/Session.o:src/Session.cpp
	@echo 'compiling session'
	g++ -g -Wall -Weffc++ -std=c++11 -c -Iinclude -o bin/Session.o src/Session.cpp

#watchable.o depands on watchable.cpp
bin/Watchable.o:src/Watchable.cpp
	@echo 'copiling watchable'
	g++ -g -Wall -Weffc++ -std=c++11 -c -Iinclude -o bin/Watchable.o src/Watchable.cpp

#clean previos compiles
clean:
	@echo 'cleaning build path'
	rm -f bin/*
