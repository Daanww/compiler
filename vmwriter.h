#ifndef VMWRITER_H
#define VMWRITER_H

enum mem_segments {
	CONST,
	ARG,
	LOCAL,
	STATIC,
	THIS,
	THAT,
	POINTER,
	TEMP
};
enum vm_arithmetic {
	ADD,
	SUB,
	NEG,
	EQ,
	GT,
	LT,
	AND,
	OR,
	NOT
};

const char *mem_segments_strings[] = {"constant", "argument", "local", "static", "this", "that", "pointer", "temp"};
const char *vm_arithmetic_strings[] = {"add", "sub", "neg", "eq", "gt", "lt", "and", "or", "not"};



void open_vm_file();
void close_vm_file();


#endif