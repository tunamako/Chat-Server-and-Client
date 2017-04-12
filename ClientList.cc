#include "ClientList.h"
#include <iostream>	
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <map>
#include <poll.h>
#include <ctime>
#include <fstream>
#include <fcntl.h>
#include <vector>


using namespace std;

void ErrorCheck(bool condition, const char *msg) {
	if (condition)
		perror(msg);
}
void writeTo(int sock, string input){
	int writelen = write(sock, input.c_str(), input.size());
	ErrorCheck(writelen == -1, "Failed to write");
}


ClientList::ClientList(int maxClients){
	this->maxClients = maxClients;
	this->logFile = new ofstream("log.txt", ios_base::app);
}

void ClientList::addClient(int fd){
	if(clientCount() >= maxClients){
		writeTo(fd, "Chatroom Full");
		close(fd);
		return;
	}

	//Determine next available name of the format "Client-x"
	int i = 0;
	while(nameInUse("Client-" + to_string(i))){
		i++;
	}
	clients.emplace(fd, "Client-" + to_string(i));
	echoMessage(fd, "Connected");
}

void ClientList::removeClient(int fd){
	echoMessage(fd, "Disconnected");
	clients.erase(fd);	
	close(fd);
}

int ClientList::readFrom(int fd){
	int readlen = read(fd, buffer, BUFSIZE - 1);
	ErrorCheck(readlen == -1, "Failed to read");

	buffer[readlen] = 0;

	//Return 0 if client disconnected. handlemsg will do the same if the client has /quit
	return (readlen == 0)
		? 0
		: handleMsg(fd, string(buffer));
}

int ClientList::handleMsg(int fd, string msg){
	msg.pop_back();
	if(msg[0] != '/'){
		echoMessage(fd, msg);
		return 1;
	}

	string command = msg.substr(0, 5);

	if(command.compare("/quit") == 0){
		return 0;
	} else if(command.compare("/name") == 0 && msg.size() > 5){
		changeName(fd,  msg.substr(6, msg.size()));
	} else if(command.compare("/file") == 0 && msg.size() > 5){
		sendFile(fd, msg.substr(6, msg.size()));
	} else {
		writeTo(fd, "Invalid command");
	}
	return 1;
}

void ClientList::echoMessage(int fd, string msg){
	msg = getTime() + clients[fd] + ": " + msg;
	log(msg);
	cout << msg << endl;
	for(auto c : clients){
		if(c.first != fd)
			writeTo(c.first, msg);
	}
}

void ClientList::changeName(int fd, string newName){
	if(nameInUse(newName)){
		writeTo(fd, "Name in use, choose another.");
	} else {
		echoMessage(fd, "Changed name to: " + newName);
		clients[fd] = newName;
	}
}

bool ClientList::nameInUse(string aName){
	for(auto c : clients){
		if(strcasecmp(c.second.c_str(), aName.c_str()) == 0)
			return true;
	}
	return false;
}

void ClientList::sendFile(int fd, string filepath){
	int file = open(filepath.c_str(), O_RDONLY);
	if (file == -1){
		writeTo(fd, "Could not open file");
		return;
	}
	int fileLen = read(file, buffer, BUFSIZE);

	while (fileLen > 0) {
		int error = write(fd, buffer, fileLen);
		if(error <= 0 || fileLen != error) {
			perror("Bad write");
			return;
		}
		fileLen = read(file, buffer, BUFSIZE);
	}
}

string ClientList::getTime(){
	time_t now;
	struct tm *localTime;
	time(&now);
	localTime = localtime(&now);

	string hour = to_string(localTime->tm_hour);
	hour = (hour.size() == 1)
		? "0" + hour
		: hour;

	string min = to_string(localTime->tm_min);
	min = (min.size() == 1)
		? "0" + min
		: min;

	return "[" + hour + ":" + min + "] ";
}

void ClientList::log(string msg){
	(*logFile)<< msg + "\r\n";
	logFile->flush();
}


int ClientList::clientCount(){
	return clients.size();
}

ClientList::~ClientList(void){}