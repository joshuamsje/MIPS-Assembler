/*
 * OpBits.c
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


#include "OpBits.h"
#include <stdio.h>
#include <inttypes.h>
#include <string.h>

#define NUMRECORDS 24

static OpBits OpTable[NUMRECORDS] = {
	{"addi", "001000"},
	{"add", "000000"},
	{"addu", "000000"},
	{"addiu", "001001"},
	{"mult", "000000"},
	{"nor", "000000"},
	{"slti", "001010"},
	{"lui", "001111"},
	{"lw", "100011"},
	{"la", "001000"},
	{"li", "001001"},
	{"sub", "000000"},
	{"mul", "011100"},
	{"beq", "000100"},
	{"bne", "000101"},
	{"slt", "000000"},
	{"sll", "000000"},
	{"sra", "000000"},
	{"sw", "101011"},
	{"srav", "000000"},
	{"j", "000010"},
	{"blez", "000110"},
	{"bgtz", "000111"},
	{"blt", "000000"},
};

char* searchOP(char* mnemonic) {
	for (int k = 0; k < NUMRECORDS; k++) {
		if (strcmp(mnemonic, OpTable[k].operation) == 0) {
			return OpTable[k].opCode;
		}
	}
	return NULL;
}

