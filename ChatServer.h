#pragma once
#include "ClientList.h"
#include <poll.h>
#include <vector>

using namespace std;

class ChatServer {
public:
	int run();
	ChatServer(char *port, int maxClients);
	~ChatServer(void);
private:
	vector<pollfd> fds;
	ClientList *clients;
	int sock;
	char *port;
	void checkNewConnections(void);
	void checkNewMessages(void);
	void makeServerSocket(void);
	int makeFileDescriptor(void);
};