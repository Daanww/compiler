#include <stdbool.h>
#include <string.h>
#include "symbol_table.h"
#include "error_handler.h"

#define TABLE_SIZE (1024 * 1024)
#define NAME_MAX_LENGTH (128)




typedef struct Data
{
	char key[NAME_MAX_LENGTH]; //name of variable
	char type[NAME_MAX_LENGTH];
	int kind;
	int running_index;
} data;

static data class_table[TABLE_SIZE] = {0,{0}};
static data subroutine_table[TABLE_SIZE] = {0,{0}};
static int static_index;
static int field_index;
static int arg_index;
static int var_index;


//based on the polynomial rolling hash function
static unsigned long compute_hash(char *string) {
	const unsigned long p = 67;
	const unsigned long m = 1e9 + 9;
	unsigned long hash_value = 0;
	unsigned long p_pow = 1;
	int character_value = 0;
	for(int i = 0; i < strlen(string); i++) {
		//following convention is used for character values
		//just use ascii code - 0, this guarantees that all digits and letters will have a value >0
		//this is quick and dirty but prob good enough for the small hash table
		character_value = string[i] - 0;
		hash_value = (hash_value + (character_value * p_pow)) % m;
		p_pow = (p_pow * p) % m;
	}
	return hash_value;
}

int get_table_index(unsigned long hash_value) {
	int table_index = hash_value % TABLE_SIZE;
	return table_index;
}

void clear_map(data *map) {
	memset(map, 0, TABLE_SIZE*sizeof(data));
	if(map == &class_table[0]) {
		static_index = 0;
		field_index = 0;
	}
	else if(map == &subroutine_table[0]) {
		arg_index = 0;
		var_index = 0;
	}
}

void start_subroutine() {
	clear_map(subroutine_table);
}

void start_class() {
	clear_map(class_table);
}

//returns number of variables of a given kind currently in scope, returns -1 on error
int get_number_of_var(int kind) {
	switch (kind)
	{
	case STATIC_KIND:
		return static_index;
		break;

	case FIELD_KIND:
		return field_index;
		break;

	case ARG_KIND:
		return arg_index;
		break;

	case VAR_KIND:
		return var_index;
		break;
	
	default:
		return -1;
		break;
	}
}

//returns enum for kind of var, returns -1 on error
int get_kind(char *name) {
	int index = contains_entry_symbol_table(name);
	if(index == -1) { //name not found in both tables
		return NONE;
	}
	else if(strcmp(name, class_table[index].key) == 0) { //found name in class_table
		return class_table[index].kind;
	}
	else if(strcmp(name, subroutine_table[index].key) == 0) { //found name in subroutine_table
		return subroutine_table[index].kind;
	}
	else {
		return -1;
	}
}

//copies type into type_buffer, return 0 on normal operation, returns -1 on error
//assumes type_buffer is large enough to store type
//leaves type_buffer empty if name is not found in current scope
int get_type(char *name, char *type_buffer) {
	int index = contains_entry_symbol_table(name);
	if(index == -1) { //name not found in both tables
		return 0;
	}
	else if(strcmp(name, class_table[index].key) == 0) { //found name in class_table
		strcpy(type_buffer, class_table[index].type);
		return 0;
	}
	else if(strcmp(name, subroutine_table[index].key) == 0) { //found name in subroutine_table
		strcpy(type_buffer, subroutine_table[index].type);
		return 0;
	}
	else {
		return -1;
	}
}

//returns assigned index of name, returns NONE is name is not found in current scope, returns -1 on error
int get_index(char *name) {
	int index = contains_entry_symbol_table(name);
	if(index == -1) { //name not found in both tables
		return NONE;
	}
	else if(strcmp(name, class_table[index].key) == 0) { //found name in class_table
		return class_table[index].running_index;
	}
	else if(strcmp(name, subroutine_table[index].key) == 0) { //found name in subroutine_table
		return subroutine_table[index].running_index;
	}
	else {
		return -1;
	}
}

//computes hash, gets table index from that and then checks if that key is at that index.
//uses linear probing to find extra slots if a collision occurs
//first checks two positions above the hashed index (index-1 and index-2) and then two positions below (index+1 index+2)
//if theres still no empty slot found or the string is not found then it throws an error
//returns index in the table of the key if it is found
//if empty=true, then it returns the index of the empty slot it finds where the key can be added
//returns -1 on error/no viable slot found
int find_index_key(char *string, data *table, bool empty) {
	int index = get_table_index(compute_hash(string));
	//check if index contains string
	if(strcmp(table[index].key, string) == 0 || (table[index].key[0] == 0 && empty)) {
		return index;
	}
	else if(strcmp(table[index-1].key, string) == 0 || (table[index-1].key[0] == 0 && empty)) {
		return index-1;
	}
	else if(strcmp(table[index-2].key, string) == 0 || (table[index-2].key[0] == 0 && empty)) {
		return index-2;
	}
	else if(strcmp(table[index+1].key, string) == 0 || (table[index+1].key[0] == 0 && empty)) {
		return index+1;
	}
	else if(strcmp(table[index+2].key, string) == 0 || (table[index+2].key[0] == 0 && empty)) {
		return index+2;
	}
	else {
		return -1;
	}

}

//returns 0 on succesfull addition, returns -1 on unsuccesfull addition
int add_entry_symbol_table(char *name, char *type, int kind) {
	if(kind == STATIC_KIND || kind == FIELD_KIND) { //add to class_table
		int running_index = 0;
		if(kind == STATIC_KIND) {
			running_index = static_index;
			static_index++;
		}
		else {
			running_index = field_index;
			field_index++;
		}
		int index = find_index_key(name, class_table, true);
		if(index != -1) {
			strcpy(class_table[index].key, name);
			strcpy(class_table[index].type, type);
			class_table[index].kind = kind;
			class_table[index].running_index = running_index;
			return 0;
		}
		else {
			return -1;
		}
	}
	else if(kind == ARG_KIND || kind == VAR_KIND) { //add to subroutine table
		int running_index = 0;
		if(kind == ARG_KIND) {
			running_index = arg_index;
			arg_index++;
		}
		else {
			running_index = var_index;
			var_index++;
		}
		int index = find_index_key(name, subroutine_table, true);
		if(index != -1) {
			strcpy(subroutine_table[index].key, name);
			strcpy(subroutine_table[index].type, type);
			subroutine_table[index].kind = kind;
			return 0;
		}
		else {
			return -1;
		}
	}
	else {
		handle_error(OTHER_ERROR, true, "Could not add identifier to symbol_table, wrong kind!");
	}
	
}

//checks if table contains a symbol and returns the index if it finds it
//returns -1 if its not found
int contains_entry_symbol_table(char *string) {
	int index = find_index_key(string, subroutine_table, false); //first check if it exists in the subroutine table
	if(index != -1) {
		return index;
	}
	else {
		index = find_index_key(string, class_table, false); //if not, check class table
		if(index != -1) {
			return index;
		}
		else {
			return -1;
		}
	}
}

