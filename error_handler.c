#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "error_handler.h"
#include "code_parser.h"


void handle_error(int error_type, bool extra_string, char* string) {
	int current_line = 0;
	char current_file[128] = {0};

	current_line = get_current_line();
	get_current_file_name(current_file, sizeof(current_file));

	const char* error_type_string = NULL;

	const char string_I_ARITHMETIC[] = "Arithmetic";
	const char string_I_PUSH[] = "Push";
	const char string_I_POP[] = "Pop";
	const char string_I_LABEL[] = "Label";
	const char string_I_GOTO[] = "Goto";
	const char string_I_IF[] = "If";
	const char string_I_FUNCTION[] = "Function";
	const char string_I_RETURN[] = "Return";
	const char string_I_CALL[] = "Call";
	const char string_OTHER_ERROR[] = "OTHER_ERROR";
	
	switch (error_type)
	{
	case I_ARITHMETIC:
		error_type_string = string_I_ARITHMETIC;
		break;

	case I_PUSH:
		error_type_string = string_I_PUSH;
		break;

	case I_POP:
		error_type_string = string_I_POP;
		break;

	case I_LABEL:
		error_type_string = string_I_LABEL;
		break;

	case I_GOTO:
		error_type_string = string_I_GOTO;
		break;

	case I_IF:
		error_type_string = string_I_IF;
		break;

	case I_FUNCTION:
		error_type_string = string_I_FUNCTION;
		break;

	case I_RETURN:
		error_type_string = string_I_RETURN;
		break;

	case I_CALL:
		error_type_string = string_I_CALL;
		break;

	case OTHER_ERROR:
		error_type_string = string_OTHER_ERROR;
		break;
	
	default:
		break;
	}

	if(extra_string) {
		printf("ERROR: In file: %s, on line: %i, %s_instruction is not valid.\nExtra Information: %s\n", current_file, current_line, error_type_string, string);
	}
	else {
		printf("ERROR: In file: %s, on line: %i, %s_instruction is not valid.\n", current_file, current_line, error_type_string);
	}

	exit(0);
}