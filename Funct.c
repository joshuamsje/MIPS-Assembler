/*
 * Funct.c
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


#include "Funct.h"
#include <stdio.h>
#include <inttypes.h>
#include <string.h>

#define NUMRECORDS 11

static Funct FunctTable[NUMRECORDS] = {
	{"mult", "011000"},
	{"mul", "000010"},
	{"sub", "100010"},
	{"add", "100000"},
	{"nor", "100111"},
	{"addu", "100001"},
	{"slt", "101010"},
	{"sll", "000000"},
	{"sra", "000011"},
	{"srav", "000111"},
	{"move", "100001"}
};

char* searchFunct(char* mnemonic1) {
	for (int k = 0; k < NUMRECORDS; k++) {
		if (strcmp(mnemonic1, FunctTable[k].rOperation) == 0) {
			return FunctTable[k].functCode;
		}
	}
	return NULL;
}
