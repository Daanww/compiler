#include <stdio.h>
#include <string.h>

#include "code_parser.h"
#include "error_handler.h"
#include "xml_labels.h"



//current xml file being written to
static FILE *xml_file;
//current line in xml file that is being written
static int current_xml_line;



//uses get_file_name() to get the name for the .xml file
void open_xml_file() {
	//open xml file
	char file_name[128] = {0};
	get_file_or_dir_name(file_name, sizeof(file_name));

	strcat(file_name, ".xml");

	xml_file = fopen(file_name, "w");
	if(xml_file == NULL) {
		handle_error(OTHER_ERROR, true, "Cannot open .xml file!");
	}
	return;
}

//writes "<tokens>"
void write_starting_code() {
	fputs(tokens_1, xml_file);
	return;
}
//writes "</tokens>"
void write_ending_code() {
	fputs(tokens_2, xml_file);
	return;
}

//writes keyword token
void write_keyword(int *enum_buffer) {
	switch (enum_buffer[1])
	{
	case CLASS:
		fputs(class_keyword, xml_file);
		break;

	case METHOD:
		fputs(method_keyword, xml_file);
		break;

	case FUNCTION:
		fputs(function_keyword, xml_file);
		break;

	case CONSTRUCTOR:
		fputs(constructor_keyword, xml_file);
		break;

	case INT:
		fputs(int_keyword, xml_file);
		break;

	case BOOLEAN:
		fputs(boolean_keyword, xml_file);
		break;

	case CHAR:
		fputs(char_keyword, xml_file);
		break;

	case VOID:
		fputs(void_keyword, xml_file);
		break;
	
	case VAR:
		fputs(var_keyword, xml_file);
		break;

	case STATIC:
		fputs(static_keyword, xml_file);
		break;

	case FIELD:
		fputs(field_keyword, xml_file);
		break;

	case LET:
		fputs(let_keyword, xml_file);
		break;

	case DO:
		fputs(do_keyword, xml_file);
		break;

	case IF:
		fputs(if_keyword, xml_file);
		break;

	case ELSE:
		fputs(else_keyword, xml_file);
		break;

	case WHILE:
		fputs(while_keyword, xml_file);
		break;

	case RETURN:
		fputs(return_keyword, xml_file);
		break;

	case TRUE:
		fputs(true_keyword, xml_file);
		break;

	case FALSE:
		fputs(false_keyword, xml_file);
		break;

	case _NULL:
		fputs(null_keyword, xml_file);
		break;

	case THIS:
		fputs(this_keyword, xml_file);
		break;

	default:
		handle_error(OTHER_ERROR, true, "Unknown keyword encountered in write_keyword()");
		break;
	}

	return;
}

//writes symbol
void write_symbol(char *string_buffer) {
	char temp_buffer[MAX_LINE_LENGTH] = {0};
	sprintf(temp_buffer, "%s %s %s", symbol_1, string_buffer, symbol_2);
	fputs(temp_buffer, xml_file);
	return;
}

//writes identifier
void write_identifier(char *string_buffer) {
	char temp_buffer[MAX_LINE_LENGTH] = {0};
	sprintf(temp_buffer, "%s %s %s", identifier_1, string_buffer, identifier_2);
	fputs(temp_buffer, xml_file);
	return;
}

//writes int_const
void write_int_const(int *enum_buffer) {
	char temp_buffer[MAX_LINE_LENGTH] = {0};
	sprintf(temp_buffer, "%s %i %s", int_const_1, enum_buffer[1], int_const_2);
	fputs(temp_buffer, xml_file);
	return;
}

//writes string_const
void write_string_const(char *string_buffer) {
	char temp_buffer[MAX_LINE_LENGTH] = {0};
	sprintf(temp_buffer, "%s %s %s", string_const_1, string_buffer, string_const_2);
	fputs(temp_buffer, xml_file);
	return;
}

//write a token to the .xml file
void write_token(int *enum_buffer, int enum_buffer_size, char *string_buffer, int string_buffer_size) {
	//checking size of buffers
	if(enum_buffer_size < 2) {
		handle_error(OTHER_ERROR, true, "Buffer to hold decoded instruction is not big enough! (size must be >=2)");
	}
	else if(string_buffer_size < MAX_SYMBOL_LENGTH) {
		handle_error(OTHER_ERROR, true, "Buffer to hold symbol_name is nog big enough! must be >=MAX_SYMBOL_LENGTH");
	}

	//writing tokens
	switch (enum_buffer[0])
	{
	case KEYWORD:
		write_keyword(enum_buffer);
		//printf("Wrote keyword with enum: %i\n", enum_buffer[1]);
		break;

	case SYMBOL:
		write_symbol(string_buffer);
		//printf("wrote symbol: %s\n", string_buffer);
		break;

	case IDENTIFIER:
		write_identifier(string_buffer);
		//printf("wrote identifier: %s\n", string_buffer);
		break;

	case INT_CONST:
		write_int_const(enum_buffer);
		//printf("wrote int_const: %i\n", enum_buffer[1]);
		break;

	case STRING_CONST:
		write_string_const(string_buffer);
		//printf("wrote string_const: %s\n", string_buffer);
		break;
	
	default:
		break;
	}


	return;
	
}


//closes the .xml file
void close_xml_file() {
	int error_status = fclose(xml_file);
	if(error_status == EOF) {
		handle_error(OTHER_ERROR, true, "Cannot close .xml file!");
	}
}