%{
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include "sms.h"

extern int yylex();
void yyerror(char *msg);

int context_level=0;

%}

%union {
  sm_double *num;
  sm_symbol *sym;
  sm_expression *expr;
  sm_array *arr;
  sm_string *str;
  sm_context * context;
};

%token CLEAR FORMAT LS NEWLINE EXIT

%type <expr> SETVAR
%type  <context>  CONTEXT
%type  <context>  CONTEXT_LIST
%type  <expr> EXPRESSION
%type  <expr> EXPRESSION_LIST
%type  <arr>  ARRAY
%token <num>  NUM
%token <sym>  SYM
%token <str>  STRING
%token <expr> SIN
%token <expr> COS
%token <expr> TAN
%token <expr> SINH
%token <expr> COSH
%token <expr> TANH
%token <expr> SEC
%token <expr> CSC
%token <expr> COT
%token <expr> LN
%token <expr> EXP
%token <expr> SQRT
%token <expr> DIFF
%token <expr> DELETE

%left  '+' '-'
%left  '*' '/'
%right '^'


%%


COMMAND : SETVAR            { }
  | DELETE SYM              {
      sm_delete((sm_symbol*)$2);
      sm_print_table(sm_global_context(NULL));
      sm_garbage_collect();
      sm_prompt();
      }
  | EXIT    ';'             { exit(0); }
  | error                   { yyerror("Bad syntax.");}
  | COMMAND ';' COMMAND ';' { }
  ;

SETVAR  : SYM '=' EXPRESSION  {

  if(context_level == 0)
  {
    sm_global_context(sm_set_var(sm_global_context(NULL),($1)->symbol_name,$3));
    sm_print_table(sm_global_context(NULL));
    sm_garbage_collect();
    sm_prompt();
  }
  else
    $$ = sm_new_expression2(sm_equals,   (sm_object*) $1, (sm_object*) $3 ) ;
}
;

EXPRESSION :EXPRESSION '-' EXPRESSION { $$ = sm_new_expression2(sm_minus,(sm_object*)$1,(sm_object*)$3); }
	| EXPRESSION '+' EXPRESSION { $$ = sm_new_expression2(sm_plus,  (sm_object*) $1, (sm_object*) $3 ) ; }
	| EXPRESSION '*' EXPRESSION { $$ = sm_new_expression2(sm_times, (sm_object*) $1, (sm_object*) $3 ) ; }
	| EXPRESSION '/' EXPRESSION { $$ = sm_new_expression2(sm_divide,(sm_object*) $1, (sm_object*) $3 ) ; }
	| EXPRESSION '^' EXPRESSION { $$ = sm_new_expression2(sm_pow,   (sm_object*) $1, (sm_object*) $3 ) ; }
	| NUM {  }
	| '-' EXPRESSION {
	    if(((sm_object*)$2)->my_type == sm_double_type){
	      ((sm_double*)$2)->value *= -1;
	      $$ = (sm_expression*)$2;
	    }
	    else {
	      $$ = sm_new_expression2(sm_times,(sm_object*)sm_new_double(-1), (sm_object*)$2 );
	    }
	  }
	| '+' EXPRESSION {$$ = (sm_expression*)$2;}
	| SYM { $$ = (sm_expression*) sm_new_symbol(($1) -> symbol_name); }
	| '(' EXPRESSION ')' { $$ = (sm_expression*) $2; }
	| STRING { }
	| SIN  '(' EXPRESSION ')' { $$ = sm_new_expression(sm_sin ,(sm_object*) $3 );}
	| COS  '(' EXPRESSION ')' { $$ = sm_new_expression(sm_cos ,(sm_object*) $3 );}
	| TAN  '(' EXPRESSION ')' { $$ = sm_new_expression(sm_tan ,(sm_object*) $3 );}
	| SINH '(' EXPRESSION ')' { $$ = sm_new_expression(sm_sinh,(sm_object*) $3 );}
	| COSH '(' EXPRESSION ')' { $$ = sm_new_expression(sm_cosh,(sm_object*) $3 );}
	| TANH '(' EXPRESSION ')' { $$ = sm_new_expression(sm_tanh,(sm_object*) $3 );}
	| SEC  '(' EXPRESSION ')' { $$ = sm_new_expression(sm_sec, (sm_object*) $3 );}
	| CSC  '(' EXPRESSION ')' { $$ = sm_new_expression(sm_csc, (sm_object*) $3 );}
	| COT  '(' EXPRESSION ')' { $$ = sm_new_expression(sm_cot, (sm_object*) $3 );}
	| LN   '(' EXPRESSION ')' { $$ = sm_new_expression(sm_ln,  (sm_object*) $3 );}
	| EXP  '(' EXPRESSION ')' { $$ = sm_new_expression(sm_exp, (sm_object*) $3 );}
	| SQRT '(' EXPRESSION ')' { $$ = sm_new_expression(sm_sqrt,(sm_object*) $3 );}
	| DIFF '(' EXPRESSION ')' { $$ = sm_new_expression(sm_diff,(sm_object*) $3 );}
	| DIFF '(' EXPRESSION ',' SYM ')' {$$ = sm_new_expression2(sm_diff,(sm_object*)$3,(sm_object*)$5 );}
	| EXPRESSION_LIST ')' { ; }
	| '[' EXPRESSION ']'	{ $$=(sm_expression*)sm_new_array_with_1((sm_object*)$2);}
	| '[' ']'							{ $$=(sm_expression*)sm_new_array(0,0);}
	| ARRAY ']' {;}
	| CONTEXT {};
	| '{' '}'							{ $$=(sm_expression*)sm_new_context(0);}

EXPRESSION_LIST: '+' '('  EXPRESSION ',' EXPRESSION  {$$ = sm_new_expression2(sm_plus,(sm_object*)$3,(sm_object*)$5 );}
  | '-' '('  EXPRESSION ',' EXPRESSION  {$$ = sm_new_expression2(sm_minus,(sm_object*)$3,(sm_object*)$5 );}
  | '*' '('  EXPRESSION ',' EXPRESSION  {$$ = sm_new_expression2(sm_times,(sm_object*)$3,(sm_object*)$5 );}
  | '/' '('  EXPRESSION ',' EXPRESSION  {$$ = sm_new_expression2(sm_divide,(sm_object*)$3,(sm_object*)$5 );}
  | EXPRESSION_LIST ',' EXPRESSION      {$$ = sm_append_to_expression((sm_expression*)$1,(sm_object*)$3);}

CONTEXT:CONTEXT_LIST '}'

CONTEXT_LIST: '{' EXPRESSION ','    {$$=sm_new_context(1);}
  | CONTEXT_LIST ',' EXPRESSION  {printf("adding more...\n"); }

ARRAY: '[' EXPRESSION ',' EXPRESSION {$$=sm_new_array_with_2((sm_object*)$2,(sm_object*)$4);}
| ARRAY ',' EXPRESSION {$$ = sm_array_append($1,(sm_object*)$3);}
%%

void yyerror(char *msg) {
	//Use this function to investigate the error.
  fprintf(stderr,"Error Specifics: %s\n",msg);
}

int main(){
  //set the current mem heap to a new mem heap
	sm_global_current_heap(sm_new_memory_heap(2500));

	//set the global context to a new context
  sm_global_context(sm_new_context(0));
  
  printf("Symbolic Math System\n");
  printf("Version 0.1\n");
  sm_prompt();

  return yyparse();
}
