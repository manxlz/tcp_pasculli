/*
 * protocol.h
 *
 *  Created on: 14 nov 2021
 *      Author: Emanuele Pasculli
 */

#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#define PROTOPORT 27016
#define STRING_LEN 8
#define ADDRESS_LEN 30

typedef struct {
	char op;
	int a;
	int b;
} operationStruct;

#endif /* PROTOCOL_H_ */
