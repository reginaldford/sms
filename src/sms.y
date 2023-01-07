%{
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include "sms.h"

extern int yylex();
void yyerror(char *msg);

%}

%union {
  sm_double *num;
  sm_symbol *sym;
  sm_expression *expr;
  sm_string *str;
  sm_context * context;
  sm_key_value * kv;
  sm_meta * meta;
};

%token CLEAR FORMAT LS NEWLINE EXIT

%type  <expr>     META
%type  <kv>       KEYVALUE
%type  <context>  CONTEXT
%type  <expr>     EXPRESSION
%type  <expr>     EXPRESSION_LIST
%type  <expr>     ARRAY
%token <num>      NUM
%token <sym>      SYM
%token <str>      STRING
%token <expr>     SIN
%token <expr>     COS
%token <expr>     TAN
%token <expr>     SINH
%token <expr>     COSH
%token <expr>     TANH
%token <expr>     SEC
%token <expr>     CSC
%token <expr>     COT
%token <expr>     LN
%token <expr>     EXP
%token <expr>     SQRT
%token <expr>     DIFF
%token <expr>     DELETE
%token <expr>     LET

%left  '+' '-'
%left  '*' '/'
%right '^'


%%


COMMAND : KEYVALUE            { 
    sm_global_context(sm_set_var(sm_global_context(NULL),$1->name,$1->value));
    sm_print_table(sm_global_context(NULL));
    sm_garbage_collect();
    sm_terminal_prompt();
  }
  | EXPRESSION {
    printf("%s\n",&(sm_object_to_string(sm_engine_eval((sm_object*)$1))->content));
    sm_garbage_collect();
    sm_terminal_prompt();
  }
  | DELETE SYM              {
      sm_delete((sm_symbol*)$2);
      sm_print_table(sm_global_context(NULL));
      sm_garbage_collect();
      sm_terminal_prompt();
  }
  | LET SYM '=' EXPRESSION  { printf("Activated let command ! (incomplete) \n"); }
  | EXIT    ';'             { sm_signal_handler(SIGQUIT); }
  | error                   { yyerror("Bad syntax.");}
  | COMMAND ';' COMMAND ';' {;}
	| META                    {
    printf("%s\n",&(sm_object_to_string(sm_engine_eval((sm_object*)$1))->content));
    sm_garbage_collect();
    sm_terminal_prompt();
	}

KEYVALUE  : SYM '=' EXPRESSION  {
    $$ = sm_new_key_value($1->name,sm_engine_eval((sm_object*)$3) ) ;
  }
	| SYM '=' META                {
    $$ = sm_new_key_value($1->name,sm_engine_eval((sm_object*)$3) ) ;
	}


META: ':' EXPRESSION {
    $$ = (sm_expression*) sm_new_meta( 1, (sm_object*) $2 );
    printf("constructed a meta\n");
  }

EXPRESSION :EXPRESSION '-' EXPRESSION { $$ = sm_new_expression_2(sm_minus,(sm_object*)$1,(sm_object*)$3); }
	| EXPRESSION '+' EXPRESSION { $$ = sm_new_expression_2(sm_plus,  (sm_object*) $1, (sm_object*) $3 ) ; }
	| EXPRESSION '*' EXPRESSION { $$ = sm_new_expression_2(sm_times, (sm_object*) $1, (sm_object*) $3 ) ; }
	| EXPRESSION '/' EXPRESSION { $$ = sm_new_expression_2(sm_divide,(sm_object*) $1, (sm_object*) $3 ) ; }
	| EXPRESSION '^' EXPRESSION { $$ = sm_new_expression_2(sm_pow,   (sm_object*) $1, (sm_object*) $3 ) ; }
	| NUM {  }
	| '-' EXPRESSION {
	    if(((sm_object*)$2)->my_type == sm_double_type){
	      ((sm_double*)$2)->value *= -1;
	      $$ = (sm_expression*)$2;
	    }
	    else {
	      $$ = sm_new_expression_2(sm_times,(sm_object*)sm_new_double(-1), (sm_object*)$2 );
	    }
	  }
	| '+' EXPRESSION {$$ = (sm_expression*)$2;}
	| SYM { $$ = (sm_expression*) sm_new_symbol(($1) -> name); }
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
	| DIFF '(' EXPRESSION ',' SYM ')' {$$ = sm_new_expression_2(sm_diff,(sm_object*)$3,(sm_object*)$5 );}
	| EXPRESSION_LIST ')' { ; }
	| '[' EXPRESSION ']'	{ $$ = (sm_expression*)sm_new_expression(sm_array,(sm_object*)$2);}
	| '{' KEYVALUE ';' '}'	{
	    $$ = (sm_expression*)sm_set_var(sm_new_context(1),$2->name,$2->value);
	  }
	| '[' ']'							{ $$ = sm_new_expression_n(sm_array,0,0);}
	| ARRAY ']' {;}
	| CONTEXT ';' '}'     {;}
	| '{' '}'							{ $$ = (sm_expression*)sm_new_context(0);}

EXPRESSION_LIST: '+' '('  EXPRESSION ',' EXPRESSION  {$$ = sm_new_expression_2(sm_plus,(sm_object*)$3,(sm_object*)$5 );}
  | '-' '('  EXPRESSION ',' EXPRESSION  {$$ = sm_new_expression_2(sm_minus,(sm_object*)$3,(sm_object*)$5 );}
  | '*' '('  EXPRESSION ',' EXPRESSION  {$$ = sm_new_expression_2(sm_times,(sm_object*)$3,(sm_object*)$5 );}
  | '/' '('  EXPRESSION ',' EXPRESSION  {$$ = sm_new_expression_2(sm_divide,(sm_object*)$3,(sm_object*)$5 );}
  | EXPRESSION_LIST ',' EXPRESSION      {$$ = sm_append_to_expression((sm_expression*)$1,(sm_object*)$3);}


ARRAY: '[' EXPRESSION ',' EXPRESSION {$$=sm_new_expression_2(sm_array,(sm_object*)$2,(sm_object*)$4);}
  | ARRAY ',' EXPRESSION {$$ = sm_append_to_expression($1,(sm_object*)$3);}


CONTEXT: '{' KEYVALUE ';' KEYVALUE {
	  $$ = sm_new_context(2);
	  $$ = sm_set_var(($$),$2->name,$2->value);
	  $$ = sm_set_var(($$),$4->name,$4->value);
  }
| CONTEXT ';' KEYVALUE {
    $$ = sm_set_var($1,$3->name,$3->value);
  }


%%

void yyerror(char *msg) {
	//Use this function to investigate the error.
  fprintf(stderr,"Error Specifics: %s\n",msg);
}

int main(){
  //Register the signal handler
  sm_register_signals();
  
  //Set the current mem heap to a new mem heap
  sm_global_current_heap(sm_new_memory_heap(5500));

  //Set the global context to a new context
  sm_global_context(sm_new_context(0));

  //Introduction and prompt
  printf("Symbolic Math System\n");
  printf("Version 0.1\n");
  sm_terminal_prompt();

  //Start the parser. 
  yyparse();

  //Exit gracefully.
  sm_signal_handler(SIGQUIT);
}
