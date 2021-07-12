#include <stdio.h>
#include <string.h>

#include "writer.h"
#include "code_parser.h"
#include "error_handler.h"
#include "xml_labels.h"



//current xml file being written to
static FILE *xml_file;
//current line in xml file that is being written
static int current_xml_line;

int token_enum_buffer[2];
char token_string_buffer[MAX_SYMBOL_LENGTH];



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
void write_token() {
	//checking size of buffers
	if((sizeof(token_enum_buffer)/sizeof(int)) < 2) {
		handle_error(OTHER_ERROR, true, "Buffer to hold decoded instruction is not big enough! (size must be >=2)");
	}
	else if((sizeof(token_string_buffer)/sizeof(char)) < MAX_SYMBOL_LENGTH) {
		handle_error(OTHER_ERROR, true, "Buffer to hold symbol_name is nog big enough! must be >=MAX_SYMBOL_LENGTH");
	}

	//writing tokens
	switch (token_enum_buffer[0])
	{
	case KEYWORD:
		write_keyword(token_enum_buffer);
		//printf("Wrote keyword with enum: %i\n", enum_buffer[1]);
		break;

	case SYMBOL:
		write_symbol(token_string_buffer);
		//printf("wrote symbol: %s\n", string_buffer);
		break;

	case IDENTIFIER:
		write_identifier(token_string_buffer);
		//printf("wrote identifier: %s\n", string_buffer);
		break;

	case INT_CONST:
		write_int_const(token_enum_buffer);
		//printf("wrote int_const: %i\n", enum_buffer[1]);
		break;

	case STRING_CONST:
		write_string_const(token_string_buffer);
		//printf("wrote string_const: %s\n", string_buffer);
		break;
	
	default:
		break;
	}


	return;
	
}

//requires an int buffer of size >= 2 and string buffer of size MAX_SYMBOL_LENGTH
//returns 1 on normal operation, returns 0 when the file has been fully emptied of tokens
int grab_token() {
	//try to decode tokens, if no tokens can be decoded then read and format next line
	while (decode_token(token_enum_buffer, sizeof(token_enum_buffer)/sizeof(int), token_string_buffer, sizeof(token_string_buffer)/sizeof(char)) == 0)
	{
		int EOF_indicator = read_line();
		if(EOF_indicator == EOF) {
			return 0;
		}
		format_line();
	}
	return 1;
}

void compile_term() {
	fputs(term_1, xml_file);

	if(token_string_buffer[0] == '(') { //found expression within braces
		write_token(); //write '('
		grab_token();
		compile_expression();
		if(token_string_buffer[0] != ')') {
			handle_error(OTHER_ERROR, true, "Could not find end of braces, missing ')'");
		}
		write_token(); //write '('
		grab_token();
		
	}
	else if(token_string_buffer[0] == '-' || token_string_buffer[0] == '~') { //found unary operator
		write_token(); //write unary operator
		grab_token();
		compile_term();
		
	}
	else {

		write_token(); //write first token of term, could be int_const, string_const or identifier etc.
		grab_token();

		if(token_string_buffer[0] == '[') { //found index symbol
			write_token(); //write '['
			grab_token();
			compile_expression();
			if(token_string_buffer[0] != ']') {
				handle_error(OTHER_ERROR, true, "Cannot find end of index in variable, missing ']'");
			}
			write_token(); //write ']'
			grab_token();
		}
		else if(token_string_buffer[0] == '(') { //found functioncall
			write_token(); //write '('
			grab_token();
			compile_expression_list();
			if(token_string_buffer[0] != ')') {
				handle_error(OTHER_ERROR, true, "Could not find end of braces, missing ')'");
			}
			write_token(); //write ')'
			grab_token();
			
		}
		else if(token_string_buffer[0] == '.') { //found functioncall
			write_token(); //write '.'
			grab_token();
			write_token(); //write identifier for function
			grab_token();
			if(token_string_buffer[0] != '(') {
				handle_error(OTHER_ERROR, true, "Could not find starting brace for expression_list in function_call, missing '('");
			}
			write_token(); //write '('
			grab_token();
			compile_expression_list();
			if(token_string_buffer[0] != ')') {
				handle_error(OTHER_ERROR, true, "Could not find ending brace for expression_list in function_call, missing ')'");
			}
			write_token(); //write ')'
			grab_token();
			
		}
	}

	fputs(term_2, xml_file);
	return;
}

void compile_expression() {
	if(token_string_buffer[0] == ')') { //empty expression
		return;
	}

	fputs(expression_1, xml_file);
	compile_term();
	while(token_string_buffer[0] == '+' || token_string_buffer[0] == '-' || token_string_buffer[0] == '*' || token_string_buffer[0] == '/' || token_string_buffer[1] == 'a' || token_string_buffer[0] == '|' || token_string_buffer[1] == 'l' || token_string_buffer[1] == 'g' || token_string_buffer[0] == '=') {
		write_token(); //write operator
		grab_token();
		compile_term();
	}
	fputs(expression_2, xml_file);
	return;
}

void compile_expression_list() {
	fputs(expression_list_1, xml_file);
	compile_expression();
	while(token_string_buffer[0] == ',') {
		write_token(); //write ','
		grab_token();
		compile_expression();
	}

	fputs(expression_list_2, xml_file);

	return;
}

void compile_return_statement() {
	fputs(return_statement_1, xml_file);
	write_token(); //write return
	grab_token();
	if(token_string_buffer[0] != ';') { //found expression
		compile_expression();
	}

	if(token_string_buffer[0] != ';') {
		handle_error(OTHER_ERROR, true, "Cannot find end symbol for return statement, missing ';'");
	}
	write_token(); //write ';'
	grab_token();

	fputs(return_statement_2, xml_file);
	return;
}

void compile_do_statement() {
	fputs(do_statement_1, xml_file);

	write_token(); //write do
	grab_token();

	write_token(); //write first token of functioncall, this is an identifier
	grab_token();

	if(token_string_buffer[0] == '(') { //simple functioncall: identifier(expression);
		write_token(); //write '('
		grab_token();
		compile_expression_list();
		if(token_string_buffer[0] != ')') {
			handle_error(OTHER_ERROR, true, "Could not find end of braces, missing ')'");
		}
		write_token(); //write ')'
		grab_token();
		
	}
	else if(token_string_buffer[0] == '.') { //complex functioncall: identifier.identifier(expression);
		write_token(); //write '.'
		grab_token();
		write_token(); //write identifier for function
		grab_token();
		if(token_string_buffer[0] != '(') {
			handle_error(OTHER_ERROR, true, "Could not find starting brace for expression_list in function_call, missing '('");
		}
		write_token(); //write '('
		grab_token();
		compile_expression_list();
		if(token_string_buffer[0] != ')') {
			handle_error(OTHER_ERROR, true, "Could not find ending brace for expression_list in function_call, missing ')'");
		}
		write_token(); //write ')'
		grab_token();
	}
	if(token_string_buffer[0] != ';') {
		handle_error(OTHER_ERROR, true, "Cannot find end symbol for do statement, missing ';'");
	}
	write_token(); //write ';'
	grab_token();

	fputs(do_statement_2, xml_file);
	return;
}

void compile_while_statement() {
	fputs(while_statement_1, xml_file);
	write_token(); //write while
	grab_token();

	if(token_string_buffer[0] != '(') {
		handle_error(OTHER_ERROR,true, "Cannot find opening brace for expression in while statement, missing '('");
	}
	write_token(); //write '('
	grab_token();
	compile_expression();
	if(token_string_buffer[0] != ')') {
		handle_error(OTHER_ERROR,true, "Cannot find ending brace for expression in while statement, missing ')'");
	}
	write_token(); //write ')'
	grab_token();

	if(token_string_buffer[0] != '{') {
		handle_error(OTHER_ERROR,true, "Cannot find opening brace for statements in while statement, missing '{'");
	}
	write_token(); //write '{'
	grab_token();
	compile_statements();
	if(token_string_buffer[0] != '}') {
		handle_error(OTHER_ERROR,true, "Cannot find ending brace for statements in while statement, missing '}'");
	}
	write_token(); //write '}'
	grab_token();
	
	fputs(while_statement_2, xml_file);
	return;
}

void compile_if_statement() {
	fputs(if_statement_1, xml_file);
	write_token(); //write if
	grab_token();

	if(token_string_buffer[0] != '(') {
		handle_error(OTHER_ERROR,true, "Cannot find opening brace for expression in if statement, missing '('");
	}
	write_token(); //write '('
	grab_token();
	compile_expression();
	if(token_string_buffer[0] != ')') {
		handle_error(OTHER_ERROR,true, "Cannot find ending brace for expression in if statement, missing ')'");
	}
	write_token(); //write ')'
	grab_token();

	if(token_string_buffer[0] != '{') {
		handle_error(OTHER_ERROR,true, "Cannot find opening brace for statements in if statement, missing '{'");
	}
	write_token(); //write '{'
	grab_token();
	compile_statements();
	if(token_string_buffer[0] != '}') {
		handle_error(OTHER_ERROR,true, "Cannot find ending brace for statements in if statement, missing '}'");
	}
	write_token(); //write '}'
	grab_token();
	if(token_enum_buffer[1] == ELSE) { //found else statement
		write_token(); //write else
		grab_token();
		if(token_string_buffer[0] != '{') {
			handle_error(OTHER_ERROR,true, "Cannot find opening brace for statements in else statement, missing '{'");
		}
		write_token(); //write '{'
		grab_token();
		compile_statements();
		if(token_string_buffer[0] != '}') {
			handle_error(OTHER_ERROR,true, "Cannot find ending brace for statements in else statement, missing '}'");
		}
		write_token(); //write '}'
		grab_token();
	}
	


	fputs(if_statement_2, xml_file);
	return;
}


void compile_let_statement() {
	fputs(let_statement_1, xml_file);

	write_token(); //write let
	grab_token();
	write_token(); //write variable name
	grab_token();
	if(token_string_buffer[0] == '[') { //found index symbol
		write_token(); //write '['
		grab_token();
		compile_expression();
		if(token_string_buffer[0] != ']') {
			handle_error(OTHER_ERROR, true, "Cannot find end of index in variable, missing ']'");
		}
		write_token(); //write ']'
		grab_token();
	}
	if(token_string_buffer[0] != '=') {
		handle_error(OTHER_ERROR, true, "Cannot find assignment operator for let statement, missing '='");
	}
	write_token(); //write '='
	grab_token();
	compile_expression();
	if(token_string_buffer[0] != ';') {
		handle_error(OTHER_ERROR, true, "Cannot find end symbol for let statement, missing ';'");
	}
	write_token(); //write ';'
	grab_token();


	fputs(let_statement_2, xml_file);
	return;
}


void compile_statements() {
	fputs(statements_1, xml_file);
	//main loop to be able to compile multiple statements
	while(token_enum_buffer[1] == LET || token_enum_buffer[1] == IF || token_enum_buffer[1] == WHILE || token_enum_buffer[1] == DO || token_enum_buffer[1] == RETURN) {
		if(token_enum_buffer[1] == LET) {
			compile_let_statement();
		}
		else if(token_enum_buffer[1] == IF) {
			compile_if_statement();
		}
		else if(token_enum_buffer[1] == WHILE) {
			compile_while_statement();
		}
		else if(token_enum_buffer[1] == DO) {
			compile_do_statement();
		}
		else if(token_enum_buffer[1] == RETURN) {
			compile_return_statement();
		}
		//grab_token();
	}

	fputs(statements_2, xml_file);
	return;
}

void compile_var_dec() {
	fputs(var_dec_1, xml_file);
	write_token(); //write var
	grab_token();
	write_token(); //write type
	grab_token();
	write_token(); //write name
	grab_token();
	while(token_string_buffer[0] == ',') {
		write_token(); //write ','
		grab_token();
		write_token(); //write name
		grab_token();
	}
	if(token_string_buffer[0] != ';') {
		handle_error(OTHER_ERROR, true, "Could not find end of variable declaration in subroutine body, missing ';'");
	}
	write_token(); //write ';'

	fputs(var_dec_2, xml_file);
	return;
}

void compile_subroutine_body() {
	fputs(subroutine_body_1, xml_file);
	write_token(); //write '{'
	grab_token();
	while(token_enum_buffer[1] == VAR) { //compile all variable declarations
		compile_var_dec();
		grab_token();
	}
	if(token_enum_buffer[1] == LET || token_enum_buffer[1] == IF || token_enum_buffer[1] == WHILE || token_enum_buffer[1] == DO || token_enum_buffer[1] == RETURN) {
		compile_statements();
	}
	else {
		handle_error(OTHER_ERROR, true, "Could not find statements, missing LET, IF, WHILE, DO, RETURN.");
	}
	if(token_string_buffer[0] != '}') {
		handle_error(OTHER_ERROR, true, "Could not find end of statements, missing '}'");
	}
	write_token(); //write '}'


	fputs(subroutine_body_2, xml_file);
	return;
}

void compile_parameter_list() {
	//if(token_string_buffer[0] == ')') { //empty parameter list
	//	return;
	//}
	fputs(parameter_list_1, xml_file);
	
	if(token_string_buffer[0] != ')')  { //there are parameters

		write_token(); //write type of first variable
		grab_token();
		write_token(); //write name of first variable
		grab_token();
		while(token_string_buffer[0] == ',') {
			write_token(); //write ','
			grab_token();
			write_token(); //write type
			grab_token();
			write_token(); //write name
			grab_token();
		}
		if(token_string_buffer[0] != ')')  {
			handle_error(OTHER_ERROR, true, "Could not find the end of the parameter_list, missing ')'");
		}
	}

	fputs(parameter_list_2, xml_file);
	return;
}

void compile_subroutine_dec() {
	fputs(subroutine_dec_1, xml_file);
	write_token(); //write constructor/function/method
	grab_token();
	write_token(); //write return type
	grab_token();
	write_token(); //write subroutine name
	grab_token();
	if(token_string_buffer[0] == '(') { //found parameter list
		write_token(); //write '('
		grab_token();
		compile_parameter_list();
	}
	else {
		handle_error(OTHER_ERROR, true, "No parameter_list found while compiling subroutine.");
	}
	if(token_string_buffer[0] != ')') { //parameter list wasnt handled correctly
		handle_error(OTHER_ERROR, true, "Parameter_list wasnt handled correctly while compiling subroutine.");
	}
	write_token(); //write ')' to end parameter list
	grab_token();
	if(token_string_buffer[0] == '{') { //found subroutine body
		compile_subroutine_body();
	}
	else {
		handle_error(OTHER_ERROR, true, "Subroutine_body was not found while compiling subroutine.");
	}

	fputs(subroutine_dec_2, xml_file);
	return;
}

void compile_class_var_dec() {
	fputs(class_var_dec_1, xml_file);
	write_token(); //write static or field
	grab_token();
	write_token(); //write the variable type
	grab_token();
	write_token(); //write the identifier
	grab_token();
	while(token_string_buffer[0] == ',') { //this means there are more variable names coming
		write_token();
		grab_token(); 
		write_token(); //write next identifier
		grab_token();
	}
	if(token_string_buffer[0] == ';') { //found end of class_var_dec
		write_token();
	}
	else {
		handle_error(OTHER_ERROR, true, "No end was found for the class_var_dec, missing ';'");
	}

	fputs(class_var_dec_2, xml_file);
	return;
}

//compiles a class, assumes only one class per source file
void compile_class() {
	grab_token();
	if(token_enum_buffer[1] == CLASS) {
		fputs(class_1, xml_file); //write <class>
		write_token(); //write class keyword
		grab_token();
		write_token(); //write identifier for class
		grab_token();
		write_token(); //write {
		grab_token();
		while (token_enum_buffer[0] == KEYWORD) //go through all stuff in the class
		{
			if(token_enum_buffer[1] == STATIC || token_enum_buffer[1] == FIELD) {
				compile_class_var_dec();
			}
			else if(token_enum_buffer[1] == CONSTRUCTOR || token_enum_buffer[1] == FUNCTION || token_enum_buffer[1] == METHOD) {
				compile_subroutine_dec();
			}
			grab_token();
		}
		if(token_string_buffer[0] == '}') { //end of class
			write_token();
			fputs(class_2, xml_file);
		}
		
	}
	else {
		handle_error(OTHER_ERROR, true, "class keyword not found!");
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