#include "ChatServer.h"
#include <stdlib.h> 
using namespace std;

int main(int argc, char *argv[]) {
	ChatServer *aServer = new ChatServer(argv[1], stoi(string(argv[2])));
	return aServer->run();
}