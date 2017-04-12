# Chat-Server-and-Client
A server and client both using poll() and connecting with TCP to achieve basic IRC-like chatroom messaging.
The server can handle as many clients as is specified at runtime, logging each message sent by each client.
Three commands are supported:
  "/name [aName]" to change a user's name
  "/quit" to disconnect
  "/file [filepath]" to access a file on the server
 
 By default, both programs connect on 127.0.0.1 with a port specified as a command line argument. The server requires a second argument to specify chatroom size.
