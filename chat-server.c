/*
	* Jigme Lobsang Lepcha 2016045
	* Aamir Tufail Ahmad 2016001
	* References: http://docs.hfbk.net/beej.us/bgipc/output/print/bgipc_A4.pdf
	* http://beej.us/guide/bgnet/pdf/bgnet_USLetter_2.pdf
	* Man pages and various internet guides
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>

#define MAX 1000
static int clients_number=1;

typedef struct 
{
	int uid;
	int addr;
	char name[64];
} msg_node;


msg_node *array_clients[MAX];

void dequeue_client(int id) {
	int i=0;
	while(i<MAX){
		if (array_clients[i] != NULL) {
			if(array_clients[i]->uid == id) {
				array_clients[i]=NULL;
				return;
			}
		}
		i++;
	}
}

char *check_online(char *buff) {
	//prints which clients are online.
	int i=0;
	memset(buff, 0, strlen(buff));
	while(i<MAX) {
		if (array_clients[i] !=  NULL) {
			sprintf(buff + strlen(buff),"%d, ",array_clients[i]->uid);
			printf("---------%d\n", array_clients[i]->uid);
		}
		i++;
	}
	printf("---Buff's value--- %s\n", buff);
	return buff;

	// for (int i = 1; i <= clients_number; i++) {
	// 	sprintf(buff, "%d", i);
	// }
	// return buff;
}

void send_message(char* msg, int uid) {
	int i=0;
	printf("sending message : %s\n",msg);
	while(i<MAX) {
		if (array_clients[i] != NULL ){
				printf("%d is current and searaching fr %d\n", array_clients[i]->uid , uid);
				if(array_clients[i]->uid == uid) {
					if (send(array_clients[i]->addr, msg, strlen(msg), 0) < 0){
						printf("%s\n", strerror(errno));
					}	
				}
		}
		i++;
	}
}

void broadcast_message(char *msg,int num) {
	int i=0;
	while(i<MAX) {
		if(array_clients[i] != NULL) {
			if (array_clients[i]->uid != num){
				send(array_clients[i]->addr, msg, strlen(msg), 0);
			}
		}
		i++;
	}
}

void *connect_to_me(void *argument){
	msg_node *client = (msg_node *)argument;
	printf("Connected to new user %d \n",client->uid);
	char str_input [100];
	while (1){
		int recieve = recv(client->addr, str_input,100,0);
		if (recieve < 0) {
			perror("Error. Not able to recieve from client.");
			exit(1);
		}
		else if (recieve > 0) {
			str_input[recieve] = '\0';
			char tuid [100];
			strcpy(tuid, str_input);
			if (strncmp(str_input, "1", 1) == 0) {
				// Personal Message
				char *token = strtok(tuid, " ");
				token++; 
				int id = atoi(token);
				if (client-> uid == id){
					send_message("Cannot Send Private Message to self",id);
				}
				else{
					send_message(&str_input[1 + strlen(token)],id);
				}
			}
			else if (strncmp(str_input, "2", 1) == 0) {
				// Broadcasting message
				broadcast_message(&str_input[1],client->uid);
			}
			else {
				// Check online users
				char buff[10000];
				check_online(buff);
				send_message(&buff, client->uid);
				continue;
			}
		}
		else{
			break;
		}
	}
		close(client->addr);
		printf("%d has left the chat\n", client->uid);
		dequeue_client(client->uid);
		pthread_detach(pthread_self());

	return NULL;
}


int main(void) {
	int server_socket, listener_socket;
	struct sockaddr_un local_address;
	int length;
	pthread_t usr_thread;
	// Initialising the server socket
	server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
	if (server_socket < 0) {
		printf("Error. Cannot initialize the socket.");
	}

	// Associates a socket address to the socket
	local_address.sun_family = AF_UNIX;
	// getcwd(local_address.sun_path, 108);
	strcpy(local_address.sun_path,"serverfilee");
	printf("%s\n", local_address.sun_path);
	unlink(local_address.sun_path);
	length = strlen(local_address.sun_path) + sizeof(local_address.sun_family);
	if (bind(server_socket, (struct sockaddr *) &local_address, length) < 0) {
		perror("Error. Cannot bind the socket.");
		return 1;
	}

	if(listen(server_socket, MAX) < 0) {
		perror("Error. Cannot listen from the socket.");
		return 1;
	}

	printf("Server is up and running.\n");
 
	struct sockaddr_un man_utd_address;
	int ssize = sizeof(struct sockaddr_un);


	while(1) {
		listener_socket = accept(server_socket, (struct sockaddr *) &man_utd_address, &ssize);

		if(listener_socket < 0) {
			perror("Error.");
			return 1;
		}

		msg_node *usr = (msg_node *)malloc(sizeof(msg_node));
		usr->addr=listener_socket;
		usr->uid=clients_number;
		array_clients[clients_number++] = usr;
		if (clients_number <=  MAX) {
			pthread_create(&usr_thread, NULL, &connect_to_me, (void *)usr);
		}

	}
	return 0;
}
