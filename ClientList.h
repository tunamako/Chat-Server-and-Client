#pragma once
#include <string>
#include <map>
#include <fstream>
#include <vector>
#include <string.h>
using namespace std;

class ClientList {
public:
	void addClient(int fd);
	void removeClient(int fd);
	int clientCount();
	int readFrom(int fd);
	ClientList(int maxClients);
	~ClientList(void);
private:
	const int BUFSIZE=1024;
	char buffer[1024];
	map<int, string> clients;
	int maxClients;
	ofstream *logFile;
	int handleMsg(int fd, string msg);
	void sendFile(int fd, string filepath);
	void echoMessage(int fd, string msg);
	void changeName(int fd, string newName);
	bool nameInUse(string aName);

	string getTime();
	void log(string msg);
};