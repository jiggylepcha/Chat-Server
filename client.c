/*
	* Jigme Lobsang Lepcha 2016045
	* Aamir Tufail Ahmad 2016001
*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>


void *rcv_msg(void *argument) {
	int *address = (int *)argument;
	char input [1000];
	while (1){
		int recieve = recv(*address, input, 1000, 0);
		if (recieve < 0) {
			perror("Error. Not able to recieve properly");
			exit(1);
		}
		else if (recieve > 0){
			input[recieve] = '\0';
			printf("%s\n", input);
		}
		else {
			break;
		}
	}
	pthread_detach(pthread_self());
	printf("Error. Server is not shutting down");
	exit(EHOSTDOWN);
	return NULL;
}


int main (void) {
	struct sockaddr_un local;
	int client_socket;
	client_socket = socket(AF_UNIX, SOCK_STREAM, 0);

	if (client_socket < 0) {
		perror("Error. Socket cannot be initialized.");
		return 1;
	}

	local.sun_family = AF_UNIX;
	// getcwd(local.sun_path, 108);
	strcpy(local.sun_path,"serverfilee");
	int length = strlen(local.sun_path) + sizeof(local.sun_family);
	if (connect (client_socket, (struct sockaddr *)&local, length) < 0) {
		perror("Error. Connection error");
		return 1;
	}

	printf("Connected to server\n");

	pthread_t client_thread;
	pthread_create(&client_thread, NULL, &rcv_msg, (void *)&client_socket);

	printf("Commands\n");
	printf("1. Choose 1 for personal message to a user\n");
	printf("2. Choose 2 for broadcasting a message\n");
	printf("3. Choose 3 to check which users are online\n");
	printf("4. Choose 4 to exit\n");
	char message [1000];
	char buff [1000];
	char option [1000];
	while(1){
		fgets(option,1000,stdin);
		switch(option[0]) {
		case '1':
			printf("Messages should be typed in this format\n");
			printf("<UID> <MESSAGE>\nWhere UID is the unique identification of the client you want to send to\n");
			fgets(message, 1000, stdin);
			message[strlen(message)-1] = '\0';
			sprintf(buff, "%d%s", 1, message);
			if (send(client_socket, buff, strlen(buff), 0) < 0){
				perror("Error. Cannot send to the client");
				return 1;
			}
			break;
		case '2':
			fgets(message, 1000, stdin);
			message[strlen(message)-1] = '\0';
			sprintf(buff, "%d%s", 2, message);
			if (send(client_socket, buff, strlen(buff), 0) < 0){
				perror("Error. Cannot send to the client");
				return 1;
			}
			break;
		case '3':	//char op[] = "3";
			{char op[]="3";
			if (send(client_socket, op, strlen(op), 0) < 0){
				perror("Error. Cannot send to the client");
				return 1;
			}
			break;}
		case '4':
			printf("%s\n", option);
			return 1;
		default:
			printf("Error. Not a valid option");
			break;
	}
	}
	close(client_socket);
	return 0;
}