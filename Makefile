all: Server Chat
Server: chat-server.c
	gcc -pthread chat-server.c -o server
Chat: client.c
	gcc -pthread client.c -o client
clean:
	rm server client 
