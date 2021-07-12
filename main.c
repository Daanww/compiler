#include <stdio.h>
#include "code_parser.h"
#include "writer.h"

int main(int argc, char *argv[]) {
    //initialization
	bool debug_mode = false;
	if(debug_mode) {
		argc = 2;
		argv[1] = "../test_folder";
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
		printf("Current jack file being read: %s\n\n", file_name);

        open_xml_file();
        
		compile_class();

        close_xml_file();
	}


    //write_ending_code(); //write ending code to xml file


    close_current_jack_file();
    //close_xml_file();
    return 0;
}
