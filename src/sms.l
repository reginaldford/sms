%{
// The following file is provided under the BSD 2-clause license. For more info, read LICENSE.txt.

#include <stdio.h>
#include <stdlib.h>
#include "sms.h"
#include "y.tab.h"

%}

%option yylineno
%option noyywrap

alpha [a-zA-Z]
digit [0-9]

%%

[ \t\f\v\n]    { ; }
exit           {return EXIT;}
clear          {return CLEAR;}
ls             {return LS;}
format         {return FORMAT;}
let            {return LET;}
self           {return SELF;}

sin         {return SIN;}
cos         {return COS;}
tan         {return TAN;}
sinh        {return SINH;}
cosh        {return COSH;}
tanh        {return TANH;}
sec         {return SEC;}
csc         {return CSC;}
cot         {return COT;}
exp         {return EXP;}
ln          {return LN;}
sqrt        {return SQRT;}
diff        {return DIFF;}
delete      {return DELETE;}

<<EOF>> {
    /* Immediately exit the program here */
    sm_signal_handler(SIGQUIT);
  };

\.?{digit}+(\.{digit}+)?([eE]-?{digit}+)?  {
      yylval.num = sm_new_double(atof(yytext));
      return NUM;
    }

[-+*/=\^\;\[\]\(\)\{\},\:]                   {return yytext[0];}
_?+{alpha}+({digit}?+{alpha}?+_?+)?+\'?+    {
      yylval.sym = sm_new_symbol(sm_new_string(yyleng,yytext));
      return SYM;
    }

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
    //use the output here
    yylval.str = sm_new_string(final_i,text);
    return STRING;
  }
%%
