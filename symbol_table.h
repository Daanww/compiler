#ifndef HASH_H
#define HASH_H

#include <stdbool.h>

enum symbol_table{
	STATIC_KIND,
	FIELD_KIND,
	ARG_KIND,
	VAR_KIND,
	NONE	
};

//contains 2 hash maps for logging variables in class and subroutine scope
//its the symbol table


void start_subroutine();
void start_class();
int get_number_of_var(int kind);
int get_kind(char *name);
int get_type(char *name, char *type_buffer);
int get_index(char *name);
int add_entry_symbol_table(char *name, char *type, int kind);
int contains_entry_symbol_table(char *string);


#endif