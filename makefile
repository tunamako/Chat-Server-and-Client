CXX = g++
CXXFLAGS = -std=c++11 -Wall -g 

main: main.o ChatServer.o ClientList.o 
	$(CXX) $(CXXFLAGS) -o main main.o ChatServer.o ClientList.o 

main.o: main.cc ChatServer.h ClientList.h
	$(CXX) $(CXXFLAGS) -c main.cc


ChatServer.o: ChatServer.cc ChatServer.h

ClientList.o: ClientList.cc ClientList.h

clean:
	rm *.o main