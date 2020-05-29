/*
 * Parser.c
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
#include "Parser.h"
#include "LabelConverter.h"
#include <inttypes.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stddef.h>
#include <math.h>

uint8_t searchTableValue(char* registerName);
char* searchTableBits(char* registerName);
char* searchOP(char* mnemonic);
char* searchFunct(char* mnemonic1);
int searchType(char* string, nameTable* table);
int searchTable(char* string, nameTable* table);
void insert(Label* label, nameTable* table);

nameTable* tableLabel;

int instruction = 0x00002000;
int counterIn = 0;
int externalIn = 0;
bool prev = true;
int newCheck = 0;

int main(int argc, char** argv)
{
	parser(argv);
	return 0;
}



int parser(char** argv)
{
	int check = 1;
	if (argv[3] != NULL)
	{
		check = strcmp("-symbols", argv[3]);
	}
	//int runner = 0;
	FILE * read;
	FILE * output;
	//creates the files
	if (check == 0)
	{
		
		read = fopen(argv[1], "r");
		output = fopen(argv[2], "w");
		int pos = symbolHandle(read, output);
		
	}
	else
	{
		read = fopen(argv[1], "r");
		output = fopen(argv[2], "w");
		int text = 0;
		if (read == NULL)
		{
			return 0;
		}
		
		char* line = calloc(100, sizeof(char*));
		char* dataOutput = calloc(300, sizeof(char*));
		tableLabel = (nameTable*)calloc(30, sizeof(nameTable));
		
		//calls in to read the first line of the file
		fgets(line, 100, read);
		
		//keeps reading through the file until 
		//reaches the data block
		while (strstr(line, ".data") == NULL)
		{
			fgets(line, 100, read);
		}
		
		fgets(line, 100, read);
		
		while ((strstr(line, ".word") != NULL) || (strstr(line, "asciiz") != NULL))
		{
			
			if (strstr(line, "asciiz") != NULL)
			{
				
				//char* result = calloc(100, sizeof(char*));
				//strcpy(result, dataDecoder(line));
				char* result = dataDecoder(line);
				strcat(dataOutput, result);
				
				free(result);
				prev = false;
				
			}
		
			if (strstr(line, ".word") != NULL)
			{
				
				//char* result = calloc(100, sizeof(char*));
				//strcpy(result, dataDecoder(line));
				char* result = dataDecoder(line);
				strcat(dataOutput, result);
				free(result);
				prev = true;
			}
			fgets(line, 100, read);
			
		}
		
		if (prev == false)
		{
			if (externalIn % 4 != 0)
			{
				int s = 4 - (externalIn % 4);
				for (int i = 0; i < s; i++)
				{
					strcat(dataOutput, "00000000");
				}
			
			}
		}
		
		
		while (strstr(line, ".text") == NULL)
		{
			fgets(line, 100, read);
			
		}
		
		while (fgets(line, 100, read) != NULL)
		{
			if (strstr(line, "$") != NULL)
			{
				text++;
				
			}
			else if (strstr(line, ":") != NULL)
			{
				if (strstr(line, "#") == NULL)
				{
					
					textLabel(line, text);
				}
				
		    }
			else if (strstr(line, "j") != NULL)
			{
				if (strstr(line, "#") == NULL)
				{
					text++;
				}
			}
			else if (strstr(line, "nop") != NULL)
			{
				text++;
			}
			
		}
		
		rewind(read);
		
		//printf("%s\n", line);
		while (strstr(line, ".text") == NULL)
		{
			fgets(line, 100, read);
			
		}
		
		while (fgets(line, 100, read) != NULL)
		{
			
			if (strstr(line, "$") != NULL)
			{
				counterIn++;
				char* result = decoder(line);
				fprintf(output, result);
				
				free(result);
				fprintf(output, "\n");
			}
			else if (strstr(line, "nop") != NULL)
			{
				counterIn++;
				
				
				fprintf(output, "00000000000000000000000000000000");
				fprintf(output, "\n");
			}
			else if (strstr(line, "j") != NULL)
			{
				if (strstr(line, "#") == NULL)
				{
					counterIn++;
				
					char* result = decoder(line);
					fprintf(output, result);
					free(result);
					fprintf(output, "\n");
				}
				
			}
			else if (strstr(line, "syscall") != NULL)
			{	
				if (strstr(line, "#") == NULL)
				{
					fprintf(output, "00000000000000000000000000001100");
					fprintf(output, "\n");
				}
			}
			
		}
		
		
		if (dataOutput[0] != '\0')
		{
			fprintf(output, "\n");
			fprintf(output, dataOutput);
		}
		
		free(line);
		free(dataOutput);
	}
	
	clear(tableLabel);
	fclose(output);
	fclose(read);
	return 0;
}

void textLabel(char* line, int count)
{
	char* temp = calloc(100, sizeof(char*));
	strcpy(temp, line);
	
	char* label = strtok(temp, " :");
	sscanf(label, "%s", label);
	Label* newLabel = calloc(1, sizeof(Label));
	newLabel->name = calloc(8, sizeof(char*));
	
	strcpy(newLabel->name, label);
	
	newLabel->address = count;
	insert(newLabel, tableLabel);
	
	free(temp);
}

char* dataDecoder(char* newLine)
{
	
	char* temp = calloc(100, sizeof(char*));
	strcpy(temp, newLine);
	char* tokenStr = strtok(temp, " :");
	char* data = strtok(NULL, " ");
	
	Label* converter = calloc(1, sizeof(Label));
	converter->name = calloc(8, sizeof(char*));
	strcpy(converter->name, tokenStr);
	newCheck++;
	if (strcmp(data, ".asciiz") == 0)
	{
		
		converter->address = instruction; 
		insert(converter, tableLabel);
		
		char* imm = strtok(NULL, " ");
		char* msg = strchr(newLine, '"');
		
		int j = strlen(msg);
		int n = strlen(msg);
		for (int i=j=0; i < n; i++)
		{
			if (msg[i] != '"')
			{
				msg[j++] = msg[i];
			}
		}
		msg[j] = '\0';
		
		int index = -1;
		int a = 0;
		while (msg[a] != '\0')
		{
			if (msg[a] != ' ' && msg[a] != '\t' && msg[a] != '\n')
			{
				index = a;
			}
			a++;
		}
		msg[index + 1] = '\0';
		
		free(temp);
		return asciiToBinary(msg);
	}
	else if (strcmp(data, ".word") == 0)
	{
		if (prev == false)
		{
			if (externalIn % 4 != 0)
			{
				int s = 4 - (externalIn % 4);
				for (int i = 0; i < s; i++)
				{
					instruction = instruction + 1;
				}
				
			}
		}
		converter->address = instruction;
		instruction = instruction + 4;
		insert(converter, tableLabel);
		
		if (strstr(newLine, ",") != NULL)
		{
		
			char* result = calloc(100, sizeof(char*));
			//char* w = strtok(NULL, " ");
			//printf("%s\n", w);
			if (prev == false)
			{
				if (externalIn % 4 != 0)
				{
					int s = 4 - (externalIn % 4);
					for (int i = 0; i < s; i++)
					{
						strcat(result, "00000000");
						externalIn++;
					}
					strcat(result, "\n");
				}
			}		
			char* new = strchr(newLine, ',') - 1;
			int a = 0;
			for (int k = 0; new[k]; k++)
			{
				if (new[k] != ' ')
				{
					new[a++] = new[k];
				
				}
			} 
			new[a] = '\0';
			sscanf(new, "%[0-9,]", new);
		
			
	
			char* first = strtok(new, ",");
			
			while (first)
			{
				int integer = atoi(first);
				char* bin = decToBinary32(integer);
				strcat(result, bin);
				strcat(result, "\n");
				first = strtok(NULL, ",");
				instruction = instruction + 4;
				free(bin);
			}
			free(temp);
			return result;
		} 
		
		char* imm = strtok(NULL, " ");
	
		if (strstr(imm, ":") != NULL)
		{
			char* finalArray = calloc(100, sizeof(char*));
			if (prev == false)
			{
				if (externalIn % 4 != 0)
				{
					int s = 4 - (externalIn % 4);
					for (int i = 0; i < s; i++)
					{
						strcat(finalArray, "00000000");
						externalIn++;
					}
					strcat(finalArray, "\n");
				}
			}	
			char* newImm = strtok(imm, ":");
			char* list = strtok(NULL, "");
	
			int listInt = atoi(list);
			int immIntNew = atoi(newImm);
		
			char* binaryArr = decToBinary32(immIntNew);
			
		
			for (int i = 0; i < listInt; i++)
			{
				instruction = instruction + 4;
				strcat(finalArray, binaryArr);
				strcat(finalArray, "\n");
			}
			instruction = instruction - 4;
			
			free(binaryArr);
			free(temp);
			
			return finalArray;
		}
		else
		{
			char* answer = calloc(50, sizeof(char*));
			if (prev == false)
			{
				if (externalIn % 4 != 0)
				{
					int s = 4 - (externalIn % 4);
					for (int i = 0; i < s; i++)
					{
						strcat(answer, "00000000");
						externalIn++;
					}
					strcat(answer, "\n");
				}
			}	
			int immInt = atoi(imm);
			char* binaryImm = decToBinary32(immInt);
			
			strcat(answer, binaryImm);
			strcat(answer, "\n"); 
			free(binaryImm);
			free(temp);
    
			return answer;
		}
		
		}
	return "";
}

char* decoder(char* newLine)
{
	char* temp = calloc(100, sizeof(char*));
	strcpy(temp, newLine);
	
	char* tokenStr;
	tokenStr = strtok(temp, " ");
	
	if ((strcmp(tokenStr, "add") == 0) || (strcmp(tokenStr, "addu") == 0))
	{
		
		char* rs = strtok(NULL, ", ");
        char* rt = strtok(NULL, ", ");
        char* rd = strtok(NULL, ", ");
        
		
        char* rsBits = searchTableBits(rs);
        char* rtBits = searchTableBits(rt);
        char* rdBits = searchTableBits(rd);
        
        char* opBits = searchOP(tokenStr);
        char* functBits = searchFunct(tokenStr);
        char* shift = "00000";
        char* result = calloc(32, sizeof(char*));
        
        
        strcat(result, opBits);
        strcat(result, rtBits);
        strcat(result, rdBits);
        strcat(result, rsBits);
        strcat(result, shift);
        strcat(result, functBits);
        
        free(temp);
        return result;
	}
	else if (strcmp(tokenStr, "sub") == 0)
	{
		char* rs = strtok(NULL, ", ");
        char* rt = strtok(NULL, ", ");
        char* rd = strtok(NULL, ", ");
        
		
        char* rsBits = searchTableBits(rs);
        char* rtBits = searchTableBits(rt);
        char* rdBits = searchTableBits(rd);
        
        char* opBits = searchOP(tokenStr);
        char* functBits = searchFunct(tokenStr);
        char* shift = "00000";
        char* result = calloc(32, sizeof(char*));
        
        
        strcat(result, opBits);
        strcat(result, rtBits);
        strcat(result, rdBits);
        strcat(result, rsBits);
        strcat(result, shift);
        strcat(result, functBits);
        
        free(temp);
       
        return result;
	}
	else if ((strcmp(tokenStr, "sll") == 0) || (strcmp(tokenStr, "sra") == 0))
	{
		char* rs = strtok(NULL, " ,");
        char* rt = strtok(NULL, " ,");
        char* imm = strtok(NULL, " ,");
        
        int immInt = atoi(imm);
        char* binaryImm = decToBinary5(immInt);
        
        
        char* opBits = searchOP(tokenStr);
        char* rsBits = searchTableBits(rs);
        char* rtBits = searchTableBits(rt);
        char* functBits = searchFunct(tokenStr);
        
        char* result = calloc(32, sizeof(char*));
        
        strcat(result, opBits);
        strcat(result, "00000");
        strcat(result, rtBits);
        strcat(result, rsBits);
        strcat(result, binaryImm);
        strcat(result, functBits);
        
        free(temp);
        free(binaryImm);
        
        return result;
        
	}
	else if ((strcmp(tokenStr, "blez") == 0) || (strcmp(tokenStr, "bgtz") == 0))
	{
		char* rs = strtok(NULL, " ,");
        
        char* theLabel = strtok(NULL, " ");
		sscanf(theLabel, "%s", theLabel);
		
		int pos = searchTable(theLabel, tableLabel);
		
		if (pos != -1)
		{
			char* opBits = searchOP(tokenStr);
			char* rsBits = searchTableBits(rs);
			int newAdd = tableLabel->array[pos].address - counterIn;

		    char* binaryImm = decToBinary(newAdd);
            char* result = calloc(32, sizeof(char*));
			
			strcat(result, opBits);
			strcat(result, rsBits);
			strcat(result, "00000");
			strcat(result, binaryImm);
			
			free(temp);
			free(binaryImm);
			return result;
		}
        return "";
        
	}
	else if (strcmp(tokenStr, "slt") == 0)
	{
		char* rs = strtok(NULL, ", ");
        char* rt = strtok(NULL, ", ");
        char* rd = strtok(NULL, ", ");
        
		
        char* rsBits = searchTableBits(rs);
        char* rtBits = searchTableBits(rt);
        char* rdBits = searchTableBits(rd);
        
        char* opBits = searchOP(tokenStr);
        char* functBits = searchFunct(tokenStr);
        char* shift = "00000";
        char* result = calloc(32, sizeof(char*));
        
        
        strcat(result, opBits);
        strcat(result, rtBits);
        strcat(result, rdBits);
        strcat(result, rsBits);
        strcat(result, shift);
        strcat(result, functBits);
        
        free(temp);
        return result;
	} 
	else if (strcmp(tokenStr, "j") == 0)
	{
		char* theLabel = strtok(NULL, " ");
		
        sscanf(theLabel, "%s", theLabel);
		int pos = searchTable(theLabel, tableLabel);
		
		if (pos != -1)
		{
			char* opBits = searchOP(tokenStr);
			int newAdd = tableLabel->array[pos].address;
		    char* binaryImm = decToBinary26(newAdd);
            char* result = calloc(32, sizeof(char*));
			
			strcat(result, opBits);
			strcat(result, binaryImm);
			
			free(temp);
			free(binaryImm);
			return result;
			
		}
       
        return "";
	} 
	else if ((strcmp(tokenStr, "addi") == 0) || (strcmp(tokenStr, "addiu") == 0))
	{
		
		char* rs = strtok(NULL, " ,");
        char* rt = strtok(NULL, " ,");
        char* imm = strtok(NULL, " ,");
        
        int immInt = atoi(imm);
        char* binaryImm = decToBinary(immInt);
        
        
        char* opBits = searchOP(tokenStr);
        char* rsBits = searchTableBits(rs);
        char* rtBits = searchTableBits(rt);
        
        
        char* result = calloc(32, sizeof(char*));
        
        strcat(result, opBits);
        strcat(result, rtBits);
        strcat(result, rsBits);
        strcat(result, binaryImm);
        
        free(temp);
        free(binaryImm);
        return result;
        
	}
	
	else if (strcmp(tokenStr, "nor") == 0)
	{
	
		char* rs = strtok(NULL, " ,");
        char* rt = strtok(NULL, " ,");
        char* rd = strtok(NULL, " ,");
        
        
        char* rsBits = searchTableBits(rs);
        char* rtBits = searchTableBits(rt);
        char* rdBits = searchTableBits(rd);
        char* opBits = searchOP(tokenStr);
        char* functBits = searchFunct(tokenStr);
        char* shift = "00000";
        char* result = calloc(32, sizeof(char*));
        
        strcat(result, opBits);
        strcat(result, rtBits);
        strcat(result, rdBits);
        strcat(result, rsBits);
        strcat(result, shift);
        strcat(result, functBits);
        
        free(temp);
        return result;
	}
	else if (strcmp(tokenStr, "slti") == 0)
	{
		char* rs = strtok(NULL, " ,");
        char* rt = strtok(NULL, " ,");
        char* imm = strtok(NULL, " ,");
        
        int immInt = atoi(imm);
        char* binaryImm = decToBinary(immInt);
        
        
        char* opBits = searchOP(tokenStr);
        char* rsBits = searchTableBits(rs);
        char* rtBits = searchTableBits(rt);
        
        
        char* result = calloc(32, sizeof(char*));
        
        strcat(result, opBits);
        strcat(result, rtBits);
        strcat(result, rsBits);
        strcat(result, binaryImm);
        
        free(temp);
        free(binaryImm);
        return result;
	}
	else if ((strcmp(tokenStr, "lw") == 0) || (strcmp(tokenStr, "sw") == 0))
	{
		//printf("lw: %s\n", temp);
		if (strstr(newLine, "(") != NULL)
		{
			char* rt = strtok(NULL, ", ");
			char* imm = strtok(NULL, "( ");
			char* rs = strtok(NULL, " )");
        
			char* opBits = searchOP(tokenStr);
			char* rsBits = searchTableBits(rs);
			char* rtBits = searchTableBits(rt);
        
			int immInt = atoi(imm);
			char* binaryImm = decToBinary(immInt);
        
			char* result = calloc(32, sizeof(char*));
        
			strcat(result, opBits);
			strcat(result, rsBits);
			strcat(result, rtBits);
			strcat(result, binaryImm);
			
			free(temp);
			free(binaryImm);
			return result;
		}
		else
		{
			char* rt = strtok(NULL, ", ");
			char* theLabel = strtok(NULL, " ");
			sscanf(theLabel, "%s", theLabel);
			int pos = searchTable(theLabel, tableLabel);
		
			if (pos != -1)
			{
				char* opBits = searchOP(tokenStr);
				char* rtBits = searchTableBits(rt);
				
				int newAdd = tableLabel->array[pos].address;
				//printf("Result: %d\n", newAdd);
				char* binaryImm = decToBinary(newAdd);
				
        
				char* result = calloc(32, sizeof(char*));
				strcat(result, opBits);
				strcat(result, "00000");
				strcat(result, rtBits);
				strcat(result, binaryImm);
				free(temp);
				free(binaryImm);
				return result;
			}
		
		}	
		//free(temp);
		return "";
	}
	else if (strcmp(tokenStr, "blt") == 0)
	{
		
		char* rs = strtok(NULL, ", ");
        char* rt = strtok(NULL, ", ");
        char* theLabel = strtok(NULL, " ");
		sscanf(theLabel, "%s", theLabel);
		int pos = searchTable(theLabel, tableLabel);
		
		if (pos != -1)
		{
			//deals with first part of the mips instruction
			//slt
			char* opBits = searchOP("slt");
			char* rdBits = searchTableBits("$at");
			char* rsBits = searchTableBits(rs);
			char* rtBits = searchTableBits(rt);
			char* functBits = searchFunct("slt");
			
			//deals with bne
			char* opBits2 = searchOP("bne");
			char* rdBits2 = searchTableBits("$at");
			char* rsBits2 = searchTableBits("$zero");
			
			int newAdd = tableLabel->array[pos].address - counterIn;
			char* binaryImm = decToBinary(newAdd);
        
			char* result = calloc(150, sizeof(char*));
        
			strcat(result, opBits);
			strcat(result, rsBits);
			strcat(result, rtBits);
			strcat(result, rdBits);
			strcat(result, "00000");
			strcat(result, functBits);
			strcat(result, "\n");
			strcat(result, opBits2);
			
			strcat(result, rdBits2);
			strcat(result, rsBits2);
			strcat(result, binaryImm);
			
			free(temp);
			free(binaryImm);
			return result;
		}
		//free(temp);
        return "";
	}
	else if (strcmp(tokenStr, "srav") == 0)
	{
		char* rs = strtok(NULL, ", ");
        char* rt = strtok(NULL, ", ");
        char* rd = strtok(NULL, ", ");
        
		
        char* rsBits = searchTableBits(rs);
        char* rtBits = searchTableBits(rt);
        char* rdBits = searchTableBits(rd);
        
        //printf("%s\n", rdBits);
        
        char* opBits = searchOP(tokenStr);
        char* functBits = searchFunct(tokenStr);
        char* shift = "00000";
        char* result = calloc(32, sizeof(char*));
        
        
        strcat(result, opBits);
        strcat(result, rdBits);
        
        strcat(result, rtBits);
        strcat(result, rsBits);
        
        strcat(result, shift);
        strcat(result, functBits);
        
        free(temp);
        return result;
	}
	else if (strcmp(tokenStr, "move") == 0)
	{
		char* rs = strtok(NULL, ", ");
        char* rt = strtok(NULL, ", ");
        
        char* rsBits = searchTableBits(rs);
        char* rtBits = searchTableBits(rt);
        char* opBits = searchOP("add");
        char* rdBits = searchTableBits("$zero");
        char* functBits = searchFunct(tokenStr);
        char* result = calloc(32, sizeof(char*));
       
        
        strcat(result, opBits);
        strcat(result, rdBits);
        strcat(result, rtBits);
        
        strcat(result, rsBits);
        
        strcat(result, "00000");
        strcat(result, functBits);
        
        free(temp);
        return result;
        
	}
	else if (strcmp(tokenStr, "mul") == 0)
	{
		char* rs = strtok(NULL, ", ");
        char* rt = strtok(NULL, ", ");
        char* rd = strtok(NULL, ", ");
        
		
        char* rsBits = searchTableBits(rs);
        char* rtBits = searchTableBits(rt);
        char* rdBits = searchTableBits(rd);
        
        //printf("%s\n", rdBits);
        
        char* opBits = searchOP(tokenStr);
        char* functBits = searchFunct(tokenStr);
        char* shift = "00000";
        char* result = calloc(32, sizeof(char*));
        
        
        strcat(result, opBits);
        strcat(result, rtBits);
        
        strcat(result, rdBits);
        strcat(result, rsBits);
        
        strcat(result, shift);
        strcat(result, functBits);
        
        free(temp);
        return result;
	}
	else if (strcmp(tokenStr, "mult") == 0)
	{
		char* rs = strtok(NULL, ", ");
        char* rt = strtok(NULL, ", ");
        
        char* rsBits = searchTableBits(rs);
        char* rtBits = searchTableBits(rt);
        
        char* opBits = searchOP(tokenStr);
        char* functBits = searchFunct(tokenStr);
        char* shift = "00000";
        char* result = calloc(32, sizeof(char*));
        
        strcat(result, opBits);
        strcat(result, rsBits);
        strcat(result, rtBits);
        strcat(result, "00000");
        strcat(result, shift);
        strcat(result, functBits);
        
        free(temp);
        return result;
	}
	else if ((strcmp(tokenStr, "lui") == 0) || (strcmp(tokenStr, "li") == 0))
	{
		char* rs = strtok(NULL, ", ");
        char* imm = strtok(NULL, ", ");
        
        char* rsBits = searchTableBits(rs);
        int immInt = atoi(imm);
        char* binaryImm = decToBinary(immInt);
        
        char* opBits = searchOP(tokenStr);
        
        char* result = calloc(32, sizeof(char*));
        
        strcat(result, opBits);
        strcat(result, "00000");
        strcat(result, rsBits);
        strcat(result, binaryImm);
        
        free(temp);
        free(binaryImm);
        return result;
	}
	else if (strcmp(tokenStr, "beq") == 0)
	{
		
		char* rs = strtok(NULL, ", ");
        char* rt = strtok(NULL, ", ");
        char* theLabel = strtok(NULL, " ");
		sscanf(theLabel, "%s", theLabel);
		
		int pos = searchTable(theLabel, tableLabel);
		
		if (pos != -1)
		{
			//printf("%s\n", theLabel);
			char* opBits = searchOP(tokenStr);
			char* rsBits = searchTableBits(rs);
			char* rtBits = searchTableBits(rt);
			
			int newAdd = tableLabel->array[pos].address - counterIn;
			char* binaryImm = decToBinary(newAdd);
        
			char* result = calloc(32, sizeof(char*));
        
			strcat(result, opBits);
			strcat(result, rsBits);
			strcat(result, rtBits);
			strcat(result, binaryImm);
			
			free(temp);
			free(binaryImm);
			return result;
		}
		//free(temp);
        return "";
	}
	
	else if (strcmp(tokenStr, "bne") == 0)
	{
		char* rs = strtok(NULL, ", ");
        char* rt = strtok(NULL, ", ");
        char* theLabel = strtok(NULL, " ");
		sscanf(theLabel, "%s", theLabel);
		int pos = searchTable(theLabel, tableLabel);
		
		if (pos != -1)
		{
			char* opBits = searchOP(tokenStr);
			char* rsBits = searchTableBits(rs);
			char* rtBits = searchTableBits(rt);
			
			int newAdd = tableLabel->array[pos].address - counterIn;
			char* binaryImm = decToBinary(newAdd);
        
			char* result = calloc(32, sizeof(char*));
			
			strcat(result, opBits);
			
			strcat(result, rsBits);
			
			strcat(result, rtBits);
			strcat(result, binaryImm);
			free(temp);
			free(binaryImm);
			return result;
			
		}
		//free(temp);
        return "";
	}
	else if (strcmp(tokenStr, "la") == 0)
	{
		char* rs = calloc(100, sizeof(char*));
		strcpy(rs, strtok(NULL, ", "));
		//char* rs = strtok(NULL, ", ");
		char* theLabel = strtok(NULL, " ");
		sscanf(theLabel, "%s", theLabel);
		int pos = searchTable(theLabel, tableLabel);
		
		if (pos != -1)
		{
			int newAdd = tableLabel->array[pos].address;
			
			char* binaryImm = decToBinary(newAdd);
			
			char* opBits = searchOP(tokenStr);
			char* rsBits = searchTableBits(rs);
			char* result = calloc(32, sizeof(char*));
			
			
			strcat(result, opBits);
			strcat(result, "00000");
			strcat(result, rsBits);
			strcat(result, binaryImm);
			//printf("Binary Rep: %s\n", binaryImm);
			
			free(temp);
			free(rs);
			free(binaryImm);
			return result;
		}
		else
		{
			//printf("off: %d\n", 1);
			char* opBits = searchOP(tokenStr);
			char* rsBits = searchTableBits(rs);
			char* result = calloc(32, sizeof(char*));
			
			int newInt = atoi(theLabel);
			char* binaryImm = decToBinary(newInt);
			
			strcat(result, opBits);
			strcat(result, "00000");
			strcat(result, rsBits);
			strcat(result, binaryImm);
			
			free(rs);
			free(temp);
			free(binaryImm);
			return result;
		}
		//free(temp);
		return "";
	}
	//free(temp);
	return "";
}

char* decToBinary(int Immediate)
{
	//allocates an array for keeping all of the bits
	//that are converted from the Immediate
	char* binaryArray = calloc(15, sizeof(char*));
	
	int result;
	//uses a for loop to get each binary digit
	//for the array
	for (int shift = 15; shift >= 0; shift--)
	{
		//performs a right shift on the immediate
		//by incrementing numbers
	    result = Immediate >> shift;
	    
	    //have the result be and by 1 to convert all the bits
	    //and also see if the bit should be 1
		if (result & 1)
		{
			binaryArray[15-shift] = '1';
			
		}
		else
		{
			//else if condition is not satisfied set the bit in the 
			//array to be zero
			binaryArray[15-shift] = '0';
			
		}
	} 
	
	return binaryArray;
}

char* decToBinary5(int Immediate)
{
	//allocates an array for keeping all of the bits
	//that are converted from the Immediate
	char* binaryArray = calloc(4, sizeof(char*));
	
	int result;
	//uses a for loop to get each binary digit
	//for the array
	for (int shift = 4; shift >= 0; shift--)
	{
		//performs a right shift on the immediate
		//by incrementing numbers
	    result = Immediate >> shift;
	    
	    //have the result be and by 1 to convert all the bits
	    //and also see if the bit should be 1
		if (result & 1)
		{
			binaryArray[4-shift] = '1';
			
		}
		else
		{
			//else if condition is not satisfied set the bit in the 
			//array to be zero
			binaryArray[4-shift] = '0';
			
		}
	} 
	
	return binaryArray;
}

char* decToBinary26(int Immediate)
{
	//allocates an array for keeping all of the bits
	//that are converted from the Immediate
	char* binaryArray = calloc(25, sizeof(char*));
	
	int result;
	//uses a for loop to get each binary digit
	//for the array
	for (int shift = 25; shift >= 0; shift--)
	{
		//performs a right shift on the immediate
		//by incrementing numbers
	    result = Immediate >> shift;
	    
	    //have the result be and by 1 to convert all the bits
	    //and also see if the bit should be 1
		if (result & 1)
		{
			binaryArray[25-shift] = '1';
			
		}
		else
		{
			//else if condition is not satisfied set the bit in the 
			//array to be zero
			binaryArray[25-shift] = '0';
			
		}
	} 
	
	return binaryArray;
}

char* decToBinary32(int Immediate)
{
	//allocates an array for keeping all of the bits
	//that are converted from the Immediate
	char* binaryArray = calloc(32, sizeof(char*));
	
	int result;
	//uses a for loop to get each binary digit
	//for the array
	for (int shift = 31; shift >= 0; shift--)
	{
		//performs a right shift on the immediate
		//by incrementing numbers
	    result = Immediate >> shift;
	    
	    //have the result be and by 1 to convert all the bits
	    //and also see if the bit should be 1
		if (result & 1)
		{
			binaryArray[31-shift] = '1';
			
		}
		else
		{
			//else if condition is not satisfied set the bit in the 
			//array to be zero
			binaryArray[31-shift] = '0';
			
		}
	} 
	
	return binaryArray;
	
}

char* asciiToBinary(char* message)
{
	int runCheck = 0;
	char* result = calloc(100, sizeof(char*));
	
	while (message[runCheck] != '\0')
	{
		
		int c = 0;
		//allocates a char array that is used for putting each
		//bit for a total of 8 bits from each ascii letter
		char* binary = calloc(8, sizeof(char*));
		
		for (int i = 7; i >= 0; i--)
		{
			//converts the first bit of the string message into
			//an int that will be inserted into the binary array
			int ascii = ((message[runCheck] & (1 << i)) ? '1' : '0');
			binary[c] = (char)ascii;
			c++;
			
		}
		//sets the instruction to be that + 1 because of the memory
		//address
		instruction = instruction + 1;
		strcat(result, binary);
		runCheck++;
		externalIn++;
		//checks to see if a new line is needed after 32 bits
		if (runCheck % 4 == 0)
		{
			strcat(result, "\n");
		}
		free(binary);
		
	}
	if (message[runCheck] == '\0')
	{
		//adds the null terminator bytes
		instruction = instruction + 1;
		strcat(result, "00000000");
		externalIn++;
	}
	
	//then checks again to see if a new line is needed at this point
	if (externalIn % 4 == 0)
	{
		strcat(result, "\n");
	}
	
	
	
	
	return result;
}


char* decToHex(int number)
{
	char* hex = calloc(100, sizeof(char*));
	char* reverse = calloc(100, sizeof(char*));
	
	int incrementer = 0; 
	int count = 0;
	
	
    while(number != 0) 
    {    
      
        int tempNumber  = 0; 
        //gets the first bit of the number
        tempNumber = number % 16; 
          
        if(tempNumber < 10) 
        { 
			//converts the first number to hex counterpart
			hex[incrementer] = tempNumber + 48; 
            incrementer++; 
            
        } 
        else
        { 
			//converts the first number to hex counterpart
            hex[incrementer] = tempNumber + 55; 
            incrementer++; 
        } 
          
        //divides the number by 16
        number = number / 16; 
    } 
    
    //reverses the hex string to make it in endian order
    for (int j = incrementer - 1; j >= 0; j--)
    {
		 reverse[count] = hex[j];
		 count++;
	}
	
	free(hex);
	return reverse;
}


int symbolHandle(FILE * in, FILE * out)
{
	int runner = 0;
	int text = 0;
	if (in == NULL)
	{
		return 0;
	}
	char* line = calloc(100, sizeof(char*));
	tableLabel = calloc(30, sizeof(nameTable));
		
	//calls in to read the first line of the file
	fgets(line, 100, in);
		
	//keeps reading through the file until 
	//reaches the data block
	while (strstr(line, ".data") == NULL)
	{
		fgets(line, 100, in);
	}
		
	fgets(line, 100, in);
	
	while ((strstr(line, ".word") != NULL) || (strstr(line, ".asciiz") != NULL))
	{
		runner++;
		
		
		
		char* temp = calloc(100, sizeof(char*));
		char* hold = calloc(100, sizeof(char*));
		strcpy(temp, line);
		strcpy(hold, line);
		char* tokenStr = strtok(temp, " :");
	
		char* data = strtok(NULL, " ");
		char* imm = strtok(NULL, " ");
	
		Label* converter = calloc(10, sizeof(Label));
		converter->name = calloc(100, sizeof(char*));
		strcpy(converter->name, tokenStr);
		
		
		if (strcmp(data, ".asciiz") == 0)
		{
			converter->address = instruction;
			insert(converter, tableLabel);
			char* msg = strchr(hold, '"');
		
			int j = strlen(msg);
			int n = strlen(msg);
			for (int i=j=0; i < n; i++)
			{
				if (msg[i] != '"')
				{
					msg[j++] = msg[i];
				}
			}
			msg[j] = '\0';
		
			int index = -1;
			int a = 0;
			while (msg[a] != '\0')
			{
				if (msg[a] != ' ' && msg[a] != '\t' && msg[a] != '\n')
				{
					index = a;
				}
				a++;
			}
			msg[index + 1] = '\0';
			char* result = asciiToBinary(msg);
			free(result);
		}
		else
		{
			if (prev == false)
			{
				if (externalIn % 4 != 0)
				{
					int pos = 4 - (externalIn % 4);
					printf("Pos: %d\n", pos);
					for (int r = 0; r < pos; r++)
					{
						instruction++;
						externalIn++;
					}
				}
			}
			converter->address = instruction;
			instruction = instruction + 4;
			insert(converter, tableLabel);
	
			if (strstr(imm, ":") != NULL)
			{
				char* newImm = strtok(imm, ":");
				char* list = strtok(NULL, "");

				int listInt = atoi(list);
				for (int i = 0; i < listInt; i++)
				{
					instruction = instruction + 4;
				}
				instruction = instruction - 4;
			}
		}
		if (strstr(line, ".asciiz") != NULL)
		{
			prev = false;
		}
		else if (strstr(line, ".word") != NULL)
		{
			prev = true;
		}
		
		free(temp);
		free(hold);
		fgets(line, 100, in);
	}

		
	for (int i = 0; i < runner; i++)
	{
		//printf("%s\n", "Infinite");
		char* nameLabel = tableLabel->array[i].name;
		printf("%s\n", nameLabel);
		int address = tableLabel->array[i].address;
	    char* hexBin = decToHex(address);
		fprintf(out, "0x0000%s", hexBin);
		if (strlen(hexBin) < 4)
		{
			int new = strlen(hexBin) % 4;
			for (int i = 0; i < new; i++)
			{
				fprintf(out, "0"); 
			}
			
		}
		
		fprintf(out, ":  %s\n", nameLabel); 
		free(hexBin);
	}
		
	while (strstr(line, ".text") == NULL)
	{
		fgets(line, 100, in);
			
	}
		
	int tempRun = runner;
	while (fgets(line, 100, in) != NULL)
	{
		if (strstr(line, "$") != NULL)
		{
			text = text + 4;
		}		
		else if (strstr(line, "j") != NULL)
		{
			if (strstr(line, "#") == NULL)
			{
				text = text + 4;
			}
			
		}	
		else if (strstr(line, "nop") != NULL)	
		{
			text = text + 4;
		}
		else if (strstr(line, ":") != NULL)
		{
			runner++;
			textLabel(line, text);
				
		}
	}
	for (int i = tempRun; i < runner; i++)
	{
		char* nameLabel = tableLabel->array[i].name;
		int address = tableLabel->array[i].address;
		char* hexBin = decToHex(address);
		fprintf(out, "0x0000");
		
		int hexLength = strlen(hexBin);
		if (hexLength % 4 != 0)
		{
			int newVal = 4 - (hexLength % 4);
			for (int j = 0; j < newVal; j++)
			{
				fprintf(out, "0");
			}
		}
		if (address == 0)
		{
			for (int t = 0; t < 4; t++)
			{
				fprintf(out, "0");
			}
		}
		fprintf(out, hexBin);
		
		fprintf(out, ":  %s\n", nameLabel); 
	}
		
	free(line);
	return 0;	
}
