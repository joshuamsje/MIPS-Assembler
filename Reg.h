// Reg.h
#ifndef REG_H
#define REG_H
#include <stdio.h>
#include <inttypes.h>
#include <stdbool.h>

struct _Reg {
	char* regName;
	uint8_t regValue;
	char* regBits;
};

typedef struct _Reg Reg;

#endif
