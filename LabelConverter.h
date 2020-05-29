#ifndef LABELCONVERTER_H
#define LABELCONVERTER_H



struct _Label{
	char* name;
	int address;
};

typedef struct _Label Label;

struct _nameTable{
	Label array[30];
	int size;
	
};

typedef struct _nameTable nameTable;


int searchTable(char* string, nameTable* table);
void insert(Label* label, nameTable* table);
void clear(nameTable* table);
#endif
