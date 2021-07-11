#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <math.h>

#include "error_handler.h"
#include "code_parser.h"

typedef struct {
	char strings[128][128];
	int num_files;
	int current_file;
} File_locations;

//maak de struct om de file_paths te bevatten
static File_locations file_locations;
//pointer to current file that is being read
static FILE *file;
//the current line that is being read
static int current_line;
//buffer to hold the current line being processed
static char line_buffer[MAX_LINE_LENGTH];

//initializes the struct for holding all the file names
//also checks if the arguments supplied are correct
//the files it searches for are specified by the arguments when the program is executed
//it searches through the whole folder if a folder is specified for all .jack files
//or it just searches for one .jack file in the current directory if a .jack file is specified.
//if debug=true, then it uses the arguments supplied inside function else it uses the arguments supplied at execution.
void index_files(int argc, char *argv[]) {

	//first check if arguments are correct
	if(argc > 2) {
		handle_error(OTHER_ERROR, true, "Too many arguments supplied.");
	}
	else if(argc < 2) {
		handle_error(OTHER_ERROR, true, "Please supply a folder or a .jack file as an argument.");
	}


	//zet alle waardes naar 0
	for(int i = 0; i < 128; i++) {
		for (int j = 0; j < 128; j++)
		{
			file_locations.strings[i][j] = 0;
		}		
	}

	//setup counters for files
	file_locations.num_files = 0;
	file_locations.current_file = -1; //set to -1 to allow open_jack_file() to increment before trying to read a file
		//this makes it smaller and more streamlined than always having current_file index be one higher than the actual current file index

	//check of argument .jack file is en check of het bestaat
	char *dot_jack_location = strstr(argv[1], ".jack");
	if(dot_jack_location != NULL) {
		//argument is een .jack file
		strcpy(file_locations.strings[0], argv[1]);
		file_locations.num_files++;
		if(file_locations.strings[0][127] != 0) { //checking for overflow
			handle_error(OTHER_ERROR, true, ".jack filename is too long. (max is 127 characters)");
		}
	}
	else {
		//check anders of directory bestaat en indexeer alle .jack files daarin

		struct dirent *directory_entry; //pointer naar struct dat de volgende directory entry in de directory stream representeerd.
		//als je de directory stream voorsteld als een trein, dan is dat een trein van wagonetjes die allemaal een dirent struct zijn
		//deze struct bevat char d_name[256], die de filename bevat, dit is wat wordt gebruikt om de .jack files te vinden in de directory

		//open een directory stream
		DIR *directory_stream = opendir(argv[1]);
		

		if(directory_stream == NULL) { //als de directory niet kon worden geopend
			handle_error(OTHER_ERROR, true, "Could not open directory given as argument.");
		}

		//reads the directory entries from directory stream
		int index = 0;
		
		while ((directory_entry = readdir(directory_stream)) != NULL)
		{
			char *dot_jack_pointer = strstr(directory_entry->d_name, ".jack");
			if(dot_jack_pointer != NULL) { //found a .jack file
				//create a temporary string to hold the path
				char file_path[128] = {0};
				strcpy(file_path, argv[1]);
				strcat(file_path, "/");
				strcat(file_path, directory_entry->d_name);

				//copy the path to the array
				strcpy(file_locations.strings[index], file_path);
				index++;
				file_locations.num_files++;

				if(file_locations.strings[index][127] != 0) { //checking for overflow
					handle_error(OTHER_ERROR, true, "Combination of directory and .jack filename is too long. (max is 127 characters)");
				}

			}
		}
		

	}

}

//used for the writer module to get the right file_name for the .jack file
//copies the right file_name from argv to buffer, also removes .jack if a single file is specified by arguments
void get_file_or_dir_name(char *buffer, int buffer_size) {

	if(buffer_size < 128) {
		handle_error(OTHER_ERROR, true, "Buffer that holds file_name for opening .asm file is not big enough! (size must be >=128).\nCalled by function get_file_name() in code_parser.c");
	}

	//finding '/' if the file path contains it and skipping all stuff before file name
	char *slash_location = NULL;
	slash_location = strrchr(file_locations.strings[file_locations.current_file], '/'); //finding last occurrence of '/'
		
	if(slash_location != NULL) { //remove all "somestuff/someotherstuff/file_name" before filename
		strcpy(buffer, (slash_location+1));
	}
	else {
		strcpy(buffer, file_locations.strings[file_locations.current_file]);
	}

	//removing ".jack" from file name if it is there
	char *dot_jack_location = NULL;
	dot_jack_location = strstr(buffer, ".jack");
	if(dot_jack_location != NULL) {
		memset(dot_jack_location, 0, 5);
	}
}

int get_total_num_files() {
	return file_locations.num_files;
}

//opens a .jack file from the file_locations array and connects it to the pointer "file".
//returns 0 on normal opening of file
//if there are no more .jack files to open, returns EOF
//handles errors associated with the .jack files
int open_next_jack_file() {
	file_locations.current_file++; //increment current file index
	current_line = 0; //reset current_line
	if(file_locations.current_file < file_locations.num_files) { //check if current_file index is lower than total num of files
		file = fopen(file_locations.strings[file_locations.current_file], "r"); //try to open file
		if(file == NULL) {
			char temp_buffer[256] = {0};
			sprintf(temp_buffer, "Could not open %s.", file_locations.strings[file_locations.current_file]);
			handle_error(OTHER_ERROR, true, temp_buffer);
		}
		return 0;
	}
	else {
		return EOF;
	}
}

//copies the name of current file being read into the buffer
void get_current_file_name(char* buffer, int buffer_size) {
	if(buffer_size < 128) {
		return;
	}
	//strncpy(buffer, file_locations.strings[file_locations.current_file], buffer_size-1);

	//finding '/' if the file path contains it and skipping all stuff before file name
	char *slash_location = NULL;
	slash_location = strrchr(file_locations.strings[file_locations.current_file], '/'); //finding last occurrence of '/'
		
	if(slash_location != NULL) { //remove all "somestuff/someotherstuff/file_name" before filename
		strcpy(buffer, (slash_location+1));
	}
	else {
		strcpy(buffer, file_locations.strings[file_locations.current_file]);
	}

	//removing ".jack" from file name if it is there
	char *dot_jack_location = NULL;
	dot_jack_location = strstr(buffer, ".jack");
	if(dot_jack_location != NULL) {
		memset(dot_jack_location, 0, 5);
	}
}

int get_current_line() {
	return current_line;
}

//returns 0 on valid line read
//returns -1 on EOF
int read_line() {
	current_line++;
	char *EOF_check = NULL;
	EOF_check = fgets(line_buffer, MAX_LINE_LENGTH, file);
	if(EOF_check == NULL) {
		printf("End of file reached!\n");
		return EOF;
	}
	if(strchr(line_buffer, '\0') == NULL) {
		handle_error(OTHER_ERROR, true, "A line in the .jack file is too long.");
	}
	printf("Read line: \"%s\"\n", line_buffer);
	return 0;
}

//formats the read line, truncating all spaces to 1 and removing comments
void format_line() {
	char new_buffer[MAX_LINE_LENGTH] = {0};
	int slash_flag = 0;
	static int long_comment_flag = 0; //long comment flag that carries over between lines, for /* */ comments
	int new_buffer_index = 0;
	int space_flag = 0;

	//remove duplicate spaces and stop if it encounters //, /* or \n
	for(int i=0; i<MAX_LINE_LENGTH; i++) {

		if(long_comment_flag != 0) { //after a /*
			if(line_buffer[i] == '/' && long_comment_flag == 2) { //checks for / in */
				long_comment_flag = 0;
			}
			
			if(line_buffer[i] == '*') { //check for * in */
				long_comment_flag = 2;
			}
			continue;
		}

		if(line_buffer[i] == '\r' || line_buffer[i] == '\n') {
			break;
		}
		if((line_buffer[i] == '/') && slash_flag) { //for handling //
			//last added character should be / so replace that with \0
			new_buffer_index--;
			new_buffer[new_buffer_index] = '\0';
			slash_flag = 0;
			break;
		}
		if((line_buffer[i] == '*') && slash_flag) { //for handling /*  */
			long_comment_flag = 1;
			new_buffer[new_buffer_index-1] = '\0';
			slash_flag = 0;
			continue;
		}

		slash_flag = 0;
		if(line_buffer[i] == '/')
			slash_flag = 1;

		if(space_flag) {
			if(line_buffer[i] == ' ')
				continue;
			else
				space_flag = 0;				
		}

		//write character to new_buffer
		if(line_buffer[i] != '\t') {
			new_buffer[new_buffer_index] = line_buffer[i];
			new_buffer_index++;
		}


		if(line_buffer[i] == ' ') {
			space_flag = 1;
		}
	}
	//removing last character in formatted line if it is a space
	if(new_buffer[new_buffer_index-1] == ' ') {
		new_buffer[new_buffer_index-1] = 0;
	}



	//copying new_buffer to line_buffer
	memcpy(line_buffer, &new_buffer, (MAX_LINE_LENGTH * sizeof(char)));

	printf("Formatted line: \"%s\"\n", line_buffer);
}

//returns the decimal number from a string if it contains a number at the end
//for example: push constant 23, this will return 23
//or: function mult 2, this will return 2
//will return -1 if it cannot find a decimal number
int get_num_value(char *string) {
	int number_value = 0;
	int string_length = strlen(string);
	int i = string_length-1;
	for(; i >= 0; i--) { 
		if(string[i] != '0' && string[i] != '1' && 
		string[i] != '2' && string[i] != '3' && 
		string[i] != '4' && string[i] != '5' && 
		string[i] != '6' && string[i] != '7' &&
		string[i] != '8' && string[i] != '9' ) //if the character is not a digit
			break;

		number_value += ((int)string[i]-48) * pow(10, string_length-i-1);
	}
	if(i == string_length-1) { //if i hasnt been modified then there was no digit present, it breaks on the first character
		return -1;
	}
	else {
		if(number_value > 32767) {
			handle_error(I_PUSH, true, "Constant must be between 0 .. 32767, inclusive.");
		} else {
			return number_value;

		}
	}
}

//decodes a single token from formatted line, it goes character by character through the line.
//for every character it checks a few things:
//first if it is a number then it keeps going for all following numbers and interpreteds that as an int constant
//if it encounters " then it knows its start of string and it interpreteds all following characters untill next " as a string constant
//next it would check if it is one of the symbols ('{', ';', '-' etc..) and interpreteds it as one
//if it is not any of the above then it is either a keyword or identifier (variable name)
//so it keeps checking the next characters, if any of them is something other than keyword/identifier then it means that all characters before are the keyword/identifier
//then it will check that bundle of chars if it is a keyword, else it is an identifier

//requires an int buffer of size >= 2 and string buffer of size MAX_SYMBOL_LENGTH
//returns 1 on normal operation, returns 0 when line_buffer is empty and a new line needs to be read
int decode_token(int *enum_buffer, int enum_buffer_size, char *string_buffer, int string_buffer_size) {
	//checking size of buffers
	if(enum_buffer_size < 2) {
		handle_error(OTHER_ERROR, true, "Buffer for holding enums in decode_token() is too small, needs size >= 2");
	}
	if(string_buffer_size < MAX_SYMBOL_LENGTH) {
		handle_error(OTHER_ERROR, true, "Buffer for holding strings in decode_token() is too small, needs size >= MAX_SYMBOL_LENGTH");
	}

	//check if line is empty
	if(line_buffer[0] == 0) {
		return 0;
	}

	//emptying both buffers
	memset(enum_buffer, 0, enum_buffer_size);
	memset(string_buffer, 0, string_buffer_size);

	char temp_buffer[MAX_LINE_LENGTH] = {0};

	//if first character of line_buffer is a space ' ' then remove that
	if(line_buffer[0] == ' ') {
		strcpy(temp_buffer, &line_buffer[1]);
		strcpy(line_buffer, temp_buffer);
		return 1;
	}


	//check for digits
	int num_digits = strspn(line_buffer, "1234567890");
	if(num_digits > 0) { //found digit
		strncpy(temp_buffer, line_buffer, num_digits); //copying all digits
		enum_buffer[0] = INT_CONST;
		enum_buffer[1] = get_num_value(temp_buffer); //getting numerical value of digits
		//getting rid of the first part of the string
		strcpy(temp_buffer, &line_buffer[num_digits]); //need num_digits index because this should point to the first character AFTER the digits
		strcpy(line_buffer, temp_buffer);
		return 1;
	}

	//check for " as declaration of string_constant
	if(line_buffer[0] == '"') { //found start of string
		char *string_end = strchr(&line_buffer[1], '"'); //finding pointer to end of the string
		int string_length = (int) (string_end - &line_buffer[1]); //getting the length of the string
		strncpy(string_buffer, &line_buffer[1], string_length); //copying string to buffer
		enum_buffer[0] = STRING_CONST;
		strcpy(temp_buffer, string_end+1); //removing string from line_buffer
		strcpy(line_buffer, temp_buffer);
		return 1;
	}

	//check for symbols
	int num_symbols = 0;
	num_symbols = strspn(line_buffer, "{}()[].,;+-*/&|<>=~");
	if(num_symbols > 0) { //found symbol
		string_buffer[0] = line_buffer[0]; //copy symbol to buffer
		if(string_buffer[0] == '<') { //manage some exceptions
			strcpy(string_buffer, "&lt;");
		}
		else if(string_buffer[0] == '>') {
			strcpy(string_buffer, "&gt;");
		}
		else if(string_buffer[0] == '&') {
			strcpy(string_buffer, "&amp;");
		}
		enum_buffer[0] = SYMBOL;
		strcpy(temp_buffer, &line_buffer[1]); //removing symbol from line_buffer
		strcpy(line_buffer, temp_buffer);
		return 1;
	}

	//keyword/identifier
	//keyword/identifier is a sequence of letter, digits and _ not starting with a digit
	//this first checks if the first character of the sequence is a letter or _ then it gets the full length of the keyword/identifier
	//then it attempts to match that substring with each of the keywords, if it matches then that is the output
	//if it matches none of the keywords then it must be an identifier and it outputs that
	int sequence_length = 0;
	sequence_length = strspn(line_buffer, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_");
	if(sequence_length > 0) {
		sequence_length = strspn(line_buffer, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_1234567890");
		strncpy(temp_buffer, line_buffer, sequence_length); //copy keyword/identifier to temp_buffer for identification
		//trying to match with keywords
		int i = 0;
		const char* keywords[] = {"class", "method", "function", "constructor", "int", "boolean", "char", "void", "var", "static", "field", "let", "do", "if", "else", "while", "return", "true", "false", "null", "this"};
		while(strcmp(keywords[i], temp_buffer) != 0) {
			i++;
			if(i > 20) //run out of strings to match
				break;
		}
		if(i < 21) { //found a keyword
			enum_buffer[0] = KEYWORD;
			enum_buffer[1] = CLASS+i; //the enums are ordered in the same way as the keywords string array thus a simple addition of index and base of the enums yields the proper enum
		}
		else { //did not find a keyword, thus it is an identifier
			enum_buffer[0] = IDENTIFIER;
			strcpy(string_buffer, temp_buffer); //copy the identifier to string_buffer
		}

		//because strcpy also copies the null terminating char temp_buffer would not need to be reset as the second strcpy call would recognize the null terminating char as the end of string
		strcpy(temp_buffer, &line_buffer[sequence_length]); //removing sequence representing keyword/identifier from line_buffer
		strcpy(line_buffer, temp_buffer);
		return 1;

	}


	return 1;

}

//close the current openend .jack file
void close_current_jack_file() {
	int error_status = fclose(file);
	if(error_status == EOF) {
		handle_error(OTHER_ERROR, true, "Could not close current .jack file!");
	}
}





