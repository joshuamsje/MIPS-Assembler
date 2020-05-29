#ifndef PARSER_H
#define PARSER_H
#include <stdio.h>

int parser(char** argv);
int main(int argc, char** argv);
char* decoder(char* newLine);
char* decToBinary(int Immediate);
char* decToBinary32(int Immediate);
char* dataDecoder(char* newLine);
void textLabel(char* line, int count);
char* decToHex(int number);
int symbolHandle(FILE * in, FILE * out);
char* dataSymbol(char* newLine);
char* decToBinary5(int Immediate);
char* decToBinary26(int Immediate);
char* asciiToBinary(char* message);
#endif
