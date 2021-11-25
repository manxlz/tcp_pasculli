/*
 ============================================================================
 Name        : server_tcp_pasculli.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
#if defined WIN32
#include <winsock.h>
#else
#define closesocket close
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "protocol.h"
#include "calculator.h"


void errorhandler(char *errorMessage) {
	printf("%s", errorMessage);
}

void clearwinsock() {
#if defined WIN32
	WSACleanup();
#endif
}



int main(int argc, char *argv[]) {

	operationStruct operation;
	boolean b = false;
	int port = 0;
	int bytes_rcvd = 0;
	int total_bytes_rcvd = 0;
	int server_socket = 0;
	int client_socket = 0;
	int client_len = 0;
	char total_string[STRING_LEN];
	float tot = 0;
	struct sockaddr_in sad;
	struct sockaddr_in cad;

	if (argc > 1) {

		sad.sin_addr.s_addr = cad.sin_addr.s_addr;
		port = atoi(argv[1]);

	} else {

		sad.sin_addr.s_addr = inet_addr("127.0.0.1");
		port = PROTOPORT;
	}

	if (port < 0) {
		printf("bad port number %s \n", argv[1]);
		return 0;
	}
#if defined WIN32
	WSADATA wsa_data;
	int result = WSAStartup(MAKEWORD(2, 2), &wsa_data);
	if (result != 0) {
		errorhandler("Error at WSAStartup()\n");
	}
#endif

	server_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (server_socket < 0) {
		errorhandler("socket creation failed.\n");
		clearwinsock();
		return -1;
	}

	memset(&sad, 0, sizeof(sad));
	sad.sin_family = AF_INET;
	sad.sin_port = htons(port);

	int c = bind(server_socket, (struct sockaddr*) &sad, sizeof(sad));

	if (c < 0) {
		errorhandler("bind() failed.\n");
		closesocket(server_socket);
		clearwinsock();
		return -1;
	}

	if (listen(server_socket, QLEN) < 0) {
		errorhandler("listen() failed.\n");
		closesocket(server_socket);
		clearwinsock();
		return -1;
	}

	while (1) {
		client_len = sizeof(cad);

		if ((client_socket = accept(server_socket, (struct sockaddr*) &cad,
				&client_len)) < 0) {
			errorhandler("accept() failed.\n");
			closesocket(server_socket);
			clearwinsock();
			return -1;
		}

		printf("Connection established with %s: %d\n", inet_ntoa(cad.sin_addr),
				ntohs(cad.sin_port));
		fflush(stdout);

		bytes_rcvd = 0, total_bytes_rcvd = 0;

		operation.op = ' ';

		//if the operation is not equal, it repeats, else the server waits a new connection
		while (operation.op != '=') {
			if ((bytes_rcvd = recv(client_socket, (char*) &operation,
					sizeof(operation), 0)) <= 0) {
				errorhandler(
						"recv() failed or connection closed prematurely.\n");
				closesocket(client_socket);
				clearwinsock();
				return -1;
			}

			total_bytes_rcvd += bytes_rcvd;

			operation.a = ntohl(operation.a);
			operation.b = ntohl(operation.b);


			if (operation.op != '=') {

				//switch for the operation of the two numbers
				switch (operation.op) {
				case '+':
					tot = add(operation.a, operation.b);
					break;
				case '-':
					tot = sub(operation.a, operation.b);
					break;
				case 'x':
					tot = mult(operation.a, operation.b);
					break;
				case '/':
					tot = division(operation.a, operation.b);
					if (operation.b == 0) {
						b = true;
					}
					break;
				}

				sprintf(total_string, "%.2f", tot);

				//b is true if there is a division by a number and 0, false otherwise
				if (b == true) {
					strcpy(total_string, "error");
					b = false;
				}

				if (send(client_socket, total_string,
						(int) sizeof(total_string), 0)
						!= sizeof(total_string)) {
					errorhandler(
							"send() sent a different number of bytes than expected");
					closesocket(server_socket);
					clearwinsock();
					return -1;

				}
				memset(total_string, 0, sizeof(total_string));
				operation.op = ' ';
				bytes_rcvd = 0;
				total_bytes_rcvd = 0;

			}

		}

	}

	fflush(stdout);
	return (0);

}
