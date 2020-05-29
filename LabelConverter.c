/*
 * LabelConverter.c
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

#include "LabelConverter.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

//search for a label, based on name
//if found return position, if nothing is found return -1
int searchTable(char* string, nameTable* table){
	for (int i = 0; i < table->size; i++)
	{
		if (strcmp(string, table->array[i].name) == 0)//, strlen(table->array[i].name)) == 0)
		{
			return i;
		}
	}
	return -1;
}



void insert(Label* label, nameTable* table){
	if (table->size < 30)
	{
		table->array[table->size] = *label;
		table->size = table->size + 1;
	}
	free(label);
}

void clear(nameTable* table)
{
	for (int i = 0; i < table->size; i++)
	{
		free(table->array[i].name);
		//free(table->array[i]);
	}
	//free(table->array);
	free(table);
}
