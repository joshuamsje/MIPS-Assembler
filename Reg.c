/*
 * Reg.c
 * 
 * Copyright 2020 Joshua Mathew <jmathew@localhost.localdomain>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */
#include "Reg.h"
#include <stdio.h>
#include <inttypes.h>
#include <string.h>

#define NUMRECORDS 26

static Reg RegisterTable[NUMRECORDS] = {
	{"$v0", 2, "00010"},
	{"$v1", 3, "00011"},
	{"$a0", 4, "00100"},
	{"$a1", 5, "00101"},
	{"$at", 1, "00001"},
	{"$s0", 16,  "10000"},
	{"$s1", 17,  "10001"},
	{"$s2", 18,  "10010"},
	{"$s3", 19,  "10011"},
	{"$s4", 20,  "10100"},
	{"$s5", 21,  "10101"},
	{"$s6", 22,  "10110"},
	{"$s7", 23,  "10111"},
	{"$t0", 8,  "01000"},
	{"$t1", 9,  "01001"},
	{"$t2", 10,  "01010"},
	{"$t3", 11,  "01011"},
	{"$t4", 12,  "01100"},
	{"$t5", 13,  "01101"},
	{"$t6", 14,  "01110"},
	{"$t7", 15,  "01111"},
	{"$t8", 24,  "11000"},
	{"$t9", 25,  "11001"},
	{"$k0", 26,  "11010"},
	{"$k1", 27,  "11011"},
	{"$zero", 0, "00000"},
};

uint8_t searchTableValue(char* registerName) {
	//finds the registervalue when given a register name from 
	//the table
	for (int k = 0; k < NUMRECORDS; k++) {
		if (strcmp(RegisterTable[k].regName, registerName) == 0) {
			return RegisterTable[k].regValue;
		}
	}
	return 0;
}

char* searchTableBits(char* registerName) {
	//finds the registervalue in bits when given a register name from 
	//the table
	for (int k = 0; k < NUMRECORDS; k++) {
		if (strstr(registerName, RegisterTable[k].regName)) {
			return RegisterTable[k].regBits;
		}
	}
	return NULL;
}

