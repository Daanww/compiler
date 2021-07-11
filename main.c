#include <stdio.h>
#include "code_parser.h"
#include "writer.h"

int main(int argc, char *argv[]) {
    //initialization
	bool debug_mode = true;
	if(debug_mode) {
		argc = 2;
		argv[1] = "../test_folder/Main.jack";
	}
	index_files(argc, argv);
	

    int end_of_jack_file_indicator = 0;

	char file_name[128] = {0};
	
	int enum_array[2] = {0};
	char name_array[MAX_SYMBOL_LENGTH] = {0};

	//write_starting_code(); //write bootstrap asm code for proper program execution
	

	//main loop
	while(true) { //looping through .jack files
		end_of_jack_file_indicator = open_next_jack_file();
		if(end_of_jack_file_indicator == EOF) {
			break;
		}
        
		get_current_file_name(file_name, sizeof(file_name));
		printf("Current jack file being read: %s\n", file_name);

        open_xml_file();
        write_starting_code(); //write beginning token code

		while(true) { //looping through lines in .jack files
			if(read_line() == EOF) {
				break;
			}
			format_line();
            while (decode_token(enum_array, sizeof(enum_array)/sizeof(int), name_array, sizeof(name_array)/sizeof(char))) //decode a token
            {
                write_token(enum_array, sizeof(enum_array)/sizeof(int), name_array, sizeof(name_array)/sizeof(char)); //write that token to xml file
            } //if decode_token() returns 0 that means that the whole line has been processed and a new line needs to be read and formatted
            
			//printf("Current line number: %i\n", get_current_line());
			//printf("Contents of enum_array: %i, %i, %i\n", enum_array[0], enum_array[1], enum_array[2]);
			//printf("Contents of name_array: %s\n", name_array);

		}
        write_ending_code(); //write ending code to xml file
        close_xml_file();
	}


    //write_ending_code(); //write ending code to xml file


    close_current_jack_file();
    //close_xml_file();
    return 0;
}
