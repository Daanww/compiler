#ifndef XML_LABELS
#define XML_LABELS

#include <string.h>

//basic classification for xml
const char tokens_1[] = "<tokens>\n";
const char tokens_2[] = "</tokens>\n";
const char symbol_1[] = "<symbol>";
const char symbol_2[] = "</symbol>\n";
const char identifier_1[] = "<identifier>";
const char identifier_2[] = "</identifier>\n";
const char int_const_1[] = "<integerConstant>";
const char int_const_2[] = "</integerConstant>\n";
const char string_const_1[] = "<stringConstant>";
const char string_const_2[] = "</stringConstant>\n";

//keywords, these can be full lines
const char class_keyword[] = "<keyword> class </keyword>\n";
const char method_keyword[] = "<keyword> method </keyword>\n";
const char function_keyword[] = "<keyword> function </keyword>\n";
const char constructor_keyword[] = "<keyword> constructor </keyword>\n";
const char int_keyword[] = "<keyword> int </keyword>\n";
const char boolean_keyword[] = "<keyword> boolean </keyword>\n";
const char char_keyword[] = "<keyword> char </keyword>\n";
const char void_keyword[] = "<keyword> void </keyword>\n";
const char var_keyword[] = "<keyword> var </keyword>\n";
const char static_keyword[] = "<keyword> static </keyword>\n";
const char field_keyword[] = "<keyword> field </keyword>\n";
const char let_keyword[] = "<keyword> let </keyword>\n";
const char do_keyword[] = "<keyword> do </keyword>\n";
const char if_keyword[] = "<keyword> if </keyword>\n";
const char else_keyword[] = "<keyword> else </keyword>\n";
const char while_keyword[] = "<keyword> while </keyword>\n";
const char return_keyword[] = "<keyword> return </keyword>\n";
const char true_keyword[] = "<keyword> true </keyword>\n";
const char false_keyword[] = "<keyword> false </keyword>\n";
const char null_keyword[] = "<keyword> null </keyword>\n";
const char this_keyword[] = "<keyword> this </keyword>\n";


#endif