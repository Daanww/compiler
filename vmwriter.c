#include <stdio.h>
#include <string.h>


#include "code_parser.h"
#include "error_handler.h"
#include "vmwriter.h"




//current vm file being written to
static FILE *vm_file;
//current line in vm file that is being written
static int current_vm_line;

void write_push(int memory_segment, int index) {
	if(memory_segment > (sizeof(mem_segments_strings)-1) || memory_segment < 0) { //if segment is invalid
		handle_error(OTHER_ERROR, true, "An invalid memory_segment has been passed to write_push().");
	}
	char temp_buffer[MAX_LINE_LENGTH] = {0};
	sprintf(temp_buffer, "push %s %i\n", mem_segments_strings[memory_segment], index);
	fputs(temp_buffer, vm_file);
	current_vm_line++;
	return;
}

void write_pop(int memory_segment, int index) {
	if(memory_segment > (sizeof(mem_segments_strings)-1) || memory_segment < 0) { //if segment is invalid
		handle_error(OTHER_ERROR, true, "An invalid memory_segment has been passed to write_pop().");
	}
	char temp_buffer[MAX_LINE_LENGTH] = {0};
	sprintf(temp_buffer, "pop %s %i\n", mem_segments_strings[memory_segment], index);
	fputs(temp_buffer, vm_file);
	current_vm_line++;
	return;
}

void write_arithmetic(int arithmetic_command) {
	if(arithmetic_command > (sizeof(vm_arithmetic_strings) - 1) || arithmetic_command < 0) { //invalid arithmetic_command
		handle_error(OTHER_ERROR, true, "An invalid arithmetic_command has been passed to write_arithmetic().");
	}
	char temp_buffer[MAX_LINE_LENGTH] = {0};
	sprintf(temp_buffer, "%s\n", vm_arithmetic_strings[arithmetic_command]);
	fputs(temp_buffer, vm_file);
	current_vm_line++;
	return;
}

void write_label(char *symbol) {
	char temp_buffer[MAX_LINE_LENGTH] = {0};
	sprintf(temp_buffer, "label %s\n", symbol);
	fputs(temp_buffer, vm_file);
	current_vm_line++;
	return;
}

void write_goto(char *symbol) {
	char temp_buffer[MAX_LINE_LENGTH] = {0};
	sprintf(temp_buffer, "goto %s\n", symbol);
	fputs(temp_buffer, vm_file);
	current_vm_line++;
	return;
}

void write_if_goto(char *symbol) {
	char temp_buffer[MAX_LINE_LENGTH] = {0};
	sprintf(temp_buffer, "if-goto %s\n", symbol);
	fputs(temp_buffer, vm_file);
	current_vm_line++;
	return;
}

void write_call(char *symbol, int n_arguments) {
	char temp_buffer[MAX_LINE_LENGTH] = {0};
	sprintf(temp_buffer, "call %s %i\n", symbol, n_arguments);
	fputs(temp_buffer, vm_file);
	current_vm_line++;
	return;
}

void write_function(char *symbol, int n_locals) {
	char temp_buffer[MAX_LINE_LENGTH] = {0};
	sprintf(temp_buffer, "function %s %i\n", symbol, n_locals);
	fputs(temp_buffer, vm_file);
	current_vm_line++;
	return;
}

void write_return() {
	fputs("return\n", vm_file);
	current_vm_line++;
	return;
}

//uses get_file_name() to get the name for the .vm file
void open_vm_file() {
	//open vm file
	char file_name[128] = {0};
	get_file_or_dir_name(file_name, sizeof(file_name));

	strcat(file_name, ".vm");

	vm_file = fopen(file_name, "w");
	if(vm_file == NULL) {
		handle_error(OTHER_ERROR, true, "Cannot open .vm file!");
	}
	current_vm_line = 0;
	return;
}

//closes the .vm file
void close_vm_file() {
	int error_status = fclose(vm_file);
	if(error_status == EOF) {
		handle_error(OTHER_ERROR, true, "Cannot close .vm file!");
	}
}