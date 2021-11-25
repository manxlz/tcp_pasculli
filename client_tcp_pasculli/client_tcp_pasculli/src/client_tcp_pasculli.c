/*
 ============================================================================
 Name        : client_tcp_pasculli.c
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
#include <ctype.h>
#include <stdbool.h>
#include "protocol.h"


void errorhandler(char *errorMessage) {
	printf("%s", errorMessage);
}

void clearwinsock() {
#if defined WIN32
	WSACleanup();
#endif
}



int main(int argc, char *argv[]) {
#if defined WIN32
	WSADATA wsa_data;
	int result = WSAStartup(MAKEWORD(2, 2), &wsa_data);
	if (result != 0) {
		errorhandler("Error at WSAStartup()\n");
		return -1;
	}
#endif

	operationStruct operation;
	int port = 0;
	int c_socket = 0;
	int bytes_rcvd = 0;
	char address[ADDRESS_LEN];
	char string_input;
	char total_string[STRING_LEN];
	char string1_input[STRING_LEN];
	char string2_input[STRING_LEN];
	struct sockaddr_in sad;

	sad.sin_addr.s_addr = inet_addr("127.0.0.1");

	if (argc > 2) {
		strcpy(address, argv[1]);
		sad.sin_addr.s_addr = inet_addr(argv[1]);
		port = atoi(argv[2]);

	} else {
		strcpy(address, "127.0.0.1");
		port = PROTOPORT;
	}

	c_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (c_socket < 0) {
		errorhandler("socket creation failed.\n");
		clearwinsock();
		return -1;
	}

	memset(&sad, 0, sizeof(sad));
	sad.sin_family = AF_INET;
	sad.sin_addr.s_addr = inet_addr(address);
	sad.sin_port = htons(port);
	if (connect(c_socket, (struct sockaddr*) &sad, sizeof(sad)) < 0) {
		errorhandler("Failed to connect.\n");
		closesocket(c_socket);
		clearwinsock();
		return -1;
	}
	operation.op = ' ';
	operation.a = 0;
	operation.b = 0;

	// it repeats while the operation is not equal, if it's true, it exit
	while (operation.op != '=') {

		printf(
				"Please insert the operation and two integer in this way: + 23 45\n");
		fflush(stdout);
		printf(
				"The operations are + for add, - for subtraction, x for multiply and / for divide\n");
		fflush(stdout);

		printf("To finish the program enter:= 0 0\n");
		fflush(stdout);

		scanf("%c %s %s", &string_input, string1_input, string2_input);
		fflush(stdin);


		if (string_input == '+' || string_input == '-' || string_input == 'x'
				|| string_input == '/' || string_input == '=') {

			operation.op = string_input;

			operation.a = atoi((char*) &string1_input);
			operation.b = atoi((char*) &string2_input);

			operation.a = htonl(operation.a);
			operation.b = htonl(operation.b);

			if (send(c_socket, (char*) &operation, sizeof(operation), 0)
					!= sizeof(operation)) {
				errorhandler(
						"send() sent a different number of bytes than expected");
				closesocket(c_socket);
				clearwinsock();
				return -1;
			}

			//if the operation is not equal, do the operation between two numbers
			if (string_input != '=') {

				bytes_rcvd = 0;


				memset(total_string, 0, sizeof(total_string));

				if ((bytes_rcvd = recv(c_socket, total_string,
						(int) sizeof(total_string), 0)) <= 0) {
					errorhandler(
							"recv() failed or connection closed prematurely.\n");
					closesocket(c_socket);
					clearwinsock();
					return -1;
				}

				total_string[bytes_rcvd] = '\0';

				// if total_string contains error means that the division is for zero
				if (strcmp(total_string, "error") == 0) {
					printf("You can't divide a number by zero.\n");
					fflush(stdout);
				} else {
					printf("The result from server: %s\n", total_string);
					fflush(stdout);
				}

				memset(total_string, 0, sizeof(total_string));
				operation.op = ' ';
				operation.a = 0;
				operation.b = 0;
				bytes_rcvd = 0;
			}
		} else {
			printf("Please insert a correct operation and try again.\n");
			fflush(stdout);
		}

	}


	printf("Client is terminated.\n");
	fflush(stdout);
	closesocket(c_socket);
	clearwinsock();
	return (0);
}
