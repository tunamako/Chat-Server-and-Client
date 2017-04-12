#include "ChatServer.h"
#include <iostream>		
#include <stdio.h>		
#include <string.h>	
#include <netinet/in.h>		
#include <unistd.h>		
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <poll.h>

using namespace std;

void ErrorCheckExit(bool condition, const char *msg) {
	if (condition){
		perror(msg);
		exit(1);
	}
}


ChatServer::ChatServer(char *port, int maxClients){
	ErrorCheckExit(port <= 0, "Invalid port");
	this->clients = new ClientList(maxClients);
	this->port = port;
	this->makeServerSocket();
	this->fds.push_back({sock, POLLIN});
}

int ChatServer::run(){
	while(true){
		int returnVal = poll(fds.data(), (nfds_t)fds.size(), -1);
		if(returnVal == 0) continue;
		if(returnVal == -1) {
			perror("Could not poll file descriptors");
			continue;
		}
		checkNewConnections();
		checkNewMessages();
	}
	close(sock);
}

void ChatServer::checkNewConnections(){
	if(fds[0].revents & POLLIN){
		int newfd = makeFileDescriptor();
		fds.push_back({newfd, POLLIN});
		clients->addClient(newfd);
	}
}

void ChatServer::checkNewMessages(){
	for(unsigned int i = 1; i < fds.size(); i++){
		if(fds[i].revents & POLLIN){
			if(clients->readFrom(fds[i].fd) == 0){
				clients->removeClient(fds[i].fd);
				fds.erase(fds.begin() + i);
			}
		}
	}
}

void ChatServer::makeServerSocket() {
	const int BACKLOG = 3;	
	int s; 		
	struct sockaddr_in sa; 
	struct hostent *hp;
	int portnum;	
	int ret;
	const char *localhostname = "127.0.0.1";

	hp = gethostbyname(localhostname);
	sscanf(this->port, "%d", &portnum);
	sa.sin_port = htons(portnum);

	bcopy((char *)hp->h_addr, (char *)&sa.sin_addr, hp->h_length);
	sa.sin_family = hp->h_addrtype;

	s = socket(hp->h_addrtype, SOCK_STREAM, 0);

	ret = bind(s, (struct sockaddr *)&sa, sizeof(sa));
	ErrorCheckExit(ret == -1, "Could not bind socket");

	listen(s, BACKLOG);
	cout << "Waiting for connection on port " << this->port << " hostname " << localhostname << endl;
	this->sock = s;
}

int ChatServer::makeFileDescriptor(){
	int sockAddrLen;
	struct sockaddr_in sockAddr;
	sockAddrLen = sizeof(sockAddr);
	return accept(this->sock, (struct sockaddr *)&sockAddr, (unsigned int *)&sockAddrLen);
}

ChatServer::~ChatServer(void){}
