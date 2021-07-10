#ifndef CODE_PARSER
#define CODE_PARSER

#include <stdbool.h>

#define MAX_LINE_LENGTH 256
#define MAX_SYMBOL_LENGTH 128

//enums for decoded instruction parts
enum token_types{
	//tokentypes:
	KEYWORD = 1,
	SYMBOL,
	IDENTIFIER,
	INT_CONST,
	STRING_CONST,

	//keywords
	CLASS,
	METHOD,
	FUNCTION,
	CONSTRUCTOR,
	INT,
	BOOLEAN,
	CHAR,
	VOID,
	VAR,
	STATIC,
	FIELD,
	LET,
	DO,
	IF,
	ELSE,
	WHILE,
	RETURN,
	TRUE,
	FALSE,
	_NULL,
	THIS
};



void index_files(int argc, char *argv[]);
void get_file_or_dir_name(char *argv[], char *buffer, int buffer_size);
int get_total_num_files();
int open_next_jack_file();
void get_current_file_name(char* buffer, int buffer_size);
int get_current_line();
int read_line();
void format_line();
void close_current_jack_file();

#endif