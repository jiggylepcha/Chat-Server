Multi-user chat system

Jigme Lobsang Lepcha 2016045
Aamir Tufail Ahmad 2016001



Description of the code:


Our multi-user chat system uses a server and a client. The clients connect to a server and can thus communicate with each other. We used a popular IPC mechanism called socket to realise this chat system.


To create a server we used the socket() system call, which will initialise the socket with the required communication protocol. We then use the bind() system call to associate a socket address to the socket. We then use the listen() system call to set the socket to wait for incoming connections. We then use the accept() system call to accept incoming connections from the client.


Each client connects to the server by using the connect() system call. Messages are sent to the server by using the send() system call and the server directs the message to the destination client by using the recv() system call.


Compiling and testing the code:


To compile the code, first run make clean to remove auxiliary files. Then run the make command to compile the program binaries. We also need to unlink the server file object by using the unlink serverfile command. After successful compilation run ./server to start the server and run ./client to join the server. Run ./client multiple times to create multiple clients.


When the client starts running there are four options which the user can make.
1. Option 1 sends a personal message to another user. Messages should be typed in the form of <UID> <Message>. For eg. 4 Hello sends Hello to the client having UID = 4.
2. Option 2 broadcasts a message to every client which is online. Messages should be typed in the form <Message>.
3. Option 3 lists the clients which are online.
4. Option 4 exits from the chat server.


Errors:


If the user selects a wrong option there is a default case to handle that. If the user types an illegal format there are cases to handle that too.


If the program exits with the error code 112, then server has crashed while there are clients still running.
