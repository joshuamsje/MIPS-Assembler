assembler: Parser.c
	gcc -std=c99 -ggdb3 -o assemble Parser.c Reg.c Funct.c OpBits.c LabelConverter.c
