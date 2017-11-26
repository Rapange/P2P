make_cpeer: cpeer.h red.h base64.cpp cpeer.cpp main.cpp
	g++ cpeer.cpp main.cpp -o main -std=c++11 -lpthread
make_server: server.cpp
	g++ server.cpp -o server -std=c++11 -lpthread
