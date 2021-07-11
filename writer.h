#ifndef WRITER
#define WRITER


void open_xml_file();
void write_starting_code();
void write_ending_code();
void write_token(int *enum_buffer, int enum_buffer_size, char *string_buffer, int string_buffer_size);
void close_xml_file();

#endif