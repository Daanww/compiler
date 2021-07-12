#ifndef WRITER
#define WRITER


void open_xml_file();
void write_token();
void compile_term();
void compile_expression();
void compile_expression_list();
void compile_return_statement();
void compile_do_statement();
void compile_while_statement();
void compile_if_statement();
void compile_let_statement();
void compile_statements();
void compile_var_dec();
void compile_subroutine_body();
void compile_parameter_list();
void compile_subroutine_dec();
void compile_class_var_dec();
void compile_class();
void close_xml_file();

#endif