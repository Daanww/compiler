#ifndef ERROR_HANDLER
#define ERROR_HANDLER

#include <stdbool.h>

//error types
enum error_types{
	I_ARITHMETIC,
	I_PUSH,
	I_POP,
	I_LABEL,
	I_GOTO,
	I_IF,
	I_FUNCTION,
	I_RETURN,
	I_CALL,
	OTHER_ERROR
};

void handle_error(int error_type, bool extra_string, char* string);

#endif