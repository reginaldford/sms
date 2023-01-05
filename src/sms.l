%{
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

\.?{digit}+(\.{digit}+)?([eE]{digit}+)?  {
      yylval.num = sm_new_double(atof(yytext));
      if(yytext[0]=='.'){
        printf("detected a leading decimal\n");
      }
      return NUM;
    }

[-+*/=\^\;\[\]\(\)\{\},]                   {return yytext[0];}
_?+{alpha}+({digit}?+{alpha}?+_?+)?+\'?+    {
      yylval.sym = sm_new_symbol(sm_new_string(yyleng,yytext));
      return SYM;
    }

\".?+\" {
    yylval.str = sm_new_string(yyleng-2,&(yytext[1]));
    return STRING;
  }

%%
