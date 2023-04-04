%{
// The following file is provided under the BSD 2-clause license. For more info, read LICENSE.txt.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../main/sms.h"
#include "y.tab.h"

%}

%option yylineno
%option noyywrap

alpha [a-zA-Z]
digit [0-9]

%%

[ \t\f\v\n] { ; }
exit        {return EXIT;}
clear       {return CLEAR;}
self        {return SELF;}
let         {return LET;}
rm          {return RM;}
"."         {return DOT;}
parent      {return PARENT;}
sin         {return SIN;}
cos         {return COS;}
tan         {return TAN;}
sinh        {return SINH;}
cosh        {return COSH;}
tanh        {return TANH;}
sec         {return SEC;}
csc         {return CSC;}
cot         {return COT;}
sech        {return SECH;}
csch        {return CSCH;}
coth        {return COTH;}
exp         {return EXP;}
ln          {return LN;}
sqrt        {return SQRT;}
abs         {return ABS;}
diff        {return DIFF;}
simp        {return SIMP;}
if          {return IF;}
map         {return MAP;}
while       {return WHILE;}
eval        {return EVAL;}
parse       {return PARSE;}
to_string   {return TO_STRING;}
==          {return EQEQ;}
=>          {return ARROW;}
"<="        {return LT_EQ;}
">="        {return GT_EQ;}
size        {return SIZE;}
print       {return PRINT;}
println     {return PRINTLN;}
input       {return INPUT;}
file_parse  {return FILE_PARSE;}
file_read   {return FILE_READ;}
file_write  {return FILE_WRITE;}
random      {return RANDOM;}
round       {return ROUND;}
not         {return NOT;}
or          {return OR;}
escape      {return ESCAPE;}
strlen      {return STRLEN;}
strcat      {return STRCAT;}
strpart     {return STRPART;}
strupper    {return STRUPPER;}
strlower    {return STRLOWER;}
strisnum    {return STRISNUM;}
<<EOF>>     {return DONE;};
'\0'        {return DONE;};

\.?{digit}+(\.{digit}+)?([eE]-?{digit}+)?  {
   yylval.num = sm_new_double(atof(yytext));
   return NUM;
  }
[\<\>]\= { return yytext[0]; }
[-+*/=\^\;\[\]\(\)\{\},\:\<\>] { return yytext[0];}
({digit}?+{alpha}?+_?+)?+\'?+ {
    yylval.sym = sm_new_symbol(sm_new_string(yyleng,yytext));
    return SYM;
  }

"//".* {/*comment*/};


\"([^"\\]|\\.)*\" {
  // Text after the first quotation will be at &(yytext[1])
  // We also have string length yyleng-2, for trimming quotes.
  char *text = &(yytext[1]) ;
  
  // Replace the escape codes in place
  int i = 0;       // Index during the scan of input string
  int final_i = 0; // Index of output
  while (i + 2 < yyleng) {
    if (text[i] == '\\') {
      // This is an escape code. Replace it with the appropriate character.
      switch (text[i + 1]) {
        case 'n':
          text[final_i] = '\n';
          break;
        case 't':
          text[final_i] = '\t';
         break;
        case 'r':
          text[final_i] = '\r';
         break;
        case '\\':
         text[final_i] = '\\';
         break;
        case '"':
         text[final_i] = '\"';
         break;
        default: // Unrecognized escape. backslash as default
         text[final_i] = '\\';
         break;
        }
        i += 2;  // Skip the escape code
      } else {
        // Pass through regular characters
        text[final_i] = text[i];
        i ++;
      }
    final_i ++;
  }
    yylval.str = sm_new_string(final_i,text);
    return STRING;
  }


%%


void _lex_file(char * fpath){
  FILE *f = fopen(fpath,"rb");
  if(access(fpath,F_OK)!=0){
    printf("File not found: %s\n",fpath);
    exit(-1);
  }
  YY_BUFFER_STATE bs = yy_create_buffer(f,YY_BUF_SIZE);
  yypush_buffer_state(bs);
}

void _done_lexing_file(){
  yypop_buffer_state();
}

void _lex_cstr(char * cstr,int len){
  yy_scan_bytes(cstr,len);
}
