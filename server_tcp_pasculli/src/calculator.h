/*
 * calculator.h
 *
 *  Created on: 14 nov 2021
 *      Author: Emanuele Pasculli
 */

#ifndef CALCULATOR_H_
#define CALCULATOR_H_

float add(float a, float b) {
	float c = 0;
	c = a + b;
	return c;
}

float sub(float a, float b) {
	float c = 0;
	c = a - b;
	return c;
}

float mult(float a, float b) {
	float c = 0;
	c = a * b;
	return c;
}

float division(float a, float b) {
	float c = 0;

	if (b == 0) {
		c = -1;
	} else {
		c = a / b;
	}
	return c;
}

#endif /* CALCULATOR_H_ */
