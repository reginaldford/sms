%{
// The following file is provided under the BSD 2-clause license. For more info, read LICENSE.txt.

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
  sm_expr *expr;
  sm_string *str;
  sm_context * context;
  sm_key_value * kv;
  sm_meta * meta;
};

%token CLEAR FORMAT LS NEWLINE EXIT SELF

%type  <expr>     FUNCALL
%type  <meta>     META_EXPR
%type  <kv>       KEYVALUE
%type  <context>  CONTEXT
%type  <context>  CONTEXT_LIST
%type  <expr>     EXPR
%type  <expr>     EXPR_LIST
%type  <expr>     ARRAY
%type  <expr>     ARRAY_LIST
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
%left ':'



%%



COMMAND : KEYVALUE            { 
    sm_global_context(sm_set_var(sm_global_context(NULL),$1->name,sm_engine_eval($1->value)));
    sm_garbage_collect();
    sm_terminal_prompt();
  }
  | EXPR {
    printf("%s\n",&(sm_object_to_string(sm_engine_eval((sm_object*)$1))->content));
    sm_garbage_collect();
    sm_terminal_prompt();
  }
  | DELETE SYM              {
      sm_delete((sm_symbol*)$2);
      sm_garbage_collect();
      sm_terminal_prompt();
  }
  | LET SYM '=' EXPR  { printf("Activated let command ! (incomplete) \n"); }
  | error                   { yyerror("Bad syntax.");}
  | COMMAND ';' COMMAND ';' {;}
  | EXIT    ';'             { sm_signal_handler(SIGQUIT); }

KEYVALUE  : SYM '=' EXPR  {
    $$ = sm_new_key_value($1->name,(sm_object*)$3 ) ;
  }

EXPR : SELF { $$ = (sm_expr*)sm_global_context(NULL); }

  | EXPR '-' EXPR { $$ = sm_new_expr_2(sm_minus,(sm_object*)$1,(sm_object*)$3); }
  | EXPR '+' EXPR { $$ = sm_new_expr_2(sm_plus,  (sm_object*) $1, (sm_object*) $3 ) ; }
  | EXPR '*' EXPR { $$ = sm_new_expr_2(sm_times, (sm_object*) $1, (sm_object*) $3 ) ; }
  | EXPR '/' EXPR { $$ = sm_new_expr_2(sm_divide,(sm_object*) $1, (sm_object*) $3 ) ; }
  | EXPR '^' EXPR { $$ = sm_new_expr_2(sm_pow,   (sm_object*) $1, (sm_object*) $3 ) ; }
  | NUM {  }
  | '-' EXPR {
  if(((sm_object*)$2)->my_type == sm_double_type){
     ((sm_double*)$2)->value *= -1;
     $$ = (sm_expr*)$2;
    }
    else {
     $$ = sm_new_expr_2(sm_times,(sm_object*)sm_new_double(-1), (sm_object*)$2 );
    }
  }
  | SYM { $$ = (sm_expr*) sm_new_symbol(($1) -> name); }
  | '(' EXPR ')' { $$ = (sm_expr*) $2; }
  | STRING { }
  | SIN  '(' EXPR ')' { $$ = sm_new_expr(sm_sin ,(sm_object*) $3 );}
  | COS  '(' EXPR ')' { $$ = sm_new_expr(sm_cos ,(sm_object*) $3 );}
  | TAN  '(' EXPR ')' { $$ = sm_new_expr(sm_tan ,(sm_object*) $3 );}
  | SINH '(' EXPR ')' { $$ = sm_new_expr(sm_sinh,(sm_object*) $3 );}
  | COSH '(' EXPR ')' { $$ = sm_new_expr(sm_cosh,(sm_object*) $3 );}
  | TANH '(' EXPR ')' { $$ = sm_new_expr(sm_tanh,(sm_object*) $3 );}
  | SEC  '(' EXPR ')' { $$ = sm_new_expr(sm_sec, (sm_object*) $3 );}
  | CSC  '(' EXPR ')' { $$ = sm_new_expr(sm_csc, (sm_object*) $3 );}
  | COT  '(' EXPR ')' { $$ = sm_new_expr(sm_cot, (sm_object*) $3 );}
  | LN   '(' EXPR ')' { $$ = sm_new_expr(sm_ln,  (sm_object*) $3 );}
  | EXP  '(' EXPR ')' { $$ = sm_new_expr(sm_exp, (sm_object*) $3 );}
  | SQRT '(' EXPR ')' { $$ = sm_new_expr(sm_sqrt,(sm_object*) $3 );}
  | DIFF '(' EXPR ')' { $$ = sm_new_expr(sm_diff,(sm_object*) $3 );}
  | DIFF '(' EXPR ',' SYM ')' {$$ = sm_new_expr_2(sm_diff,(sm_object*)$3,(sm_object*)$5 );}
  | EXPR_LIST ')' { ; }
  | CONTEXT   {;}
  | '[' EXPR ']'	{ $$ = (sm_expr*)sm_new_expr(sm_array,(sm_object*)$2);}
  | '[' ']'							{ $$ = sm_new_expr_n(sm_array,0,0);}
  | ARRAY     {;}
  | META_EXPR {;}
  | KEYVALUE {
    $$ =(sm_expr*) $1->value;
    sm_global_context(sm_set_var(sm_global_context(NULL),$1->name,sm_engine_eval($1->value)));
  }
  | FUNCALL   {;}
  
EXPR_LIST: '+' '('  EXPR ',' EXPR  {$$ = sm_new_expr_2(sm_plus,(sm_object*)$3,(sm_object*)$5 );}
  | '-' '('  EXPR ',' EXPR  {$$ = sm_new_expr_2(sm_minus,(sm_object*)$3,(sm_object*)$5 );}
  | '*' '('  EXPR ',' EXPR  {$$ = sm_new_expr_2(sm_times,(sm_object*)$3,(sm_object*)$5 );}
  | '/' '('  EXPR ',' EXPR  {$$ = sm_new_expr_2(sm_divide,(sm_object*)$3,(sm_object*)$5 );}
  | EXPR_LIST ',' EXPR      {$$ = sm_append_to_expr((sm_expr*)$1,(sm_object*)$3);}

FUNCALL: META_EXPR CONTEXT {
    ($2)->parent = sm_global_context($2);
    $$=(sm_expr*)sm_engine_eval(((sm_meta*)$1)->address);
    sm_global_context(($2)->parent);
  }
  | META_EXPR SYM {
    sm_string    *var_name = ($2)->name;
    sm_search_result sr       = sm_find_var_index(sm_global_context(NULL), var_name);
    if (sr.found == true) {
      sm_object * found = sm_context_entries(sm_global_context(NULL))[sr.index].value;
      if(found->my_type==sm_context_type){
        sm_context *found_cx = (sm_context*)found;
        found_cx->parent = sm_global_context(found_cx);
        $$=(sm_expr*)sm_engine_eval(((sm_meta*)$1)->address);
        sm_global_context(found_cx->parent);
      } else {
          printf("Variable is not a context: %s ", &(var_name->content));
          printf("If a variable follows a meta expression, the variable must point to a context.\n");
          $$= (sm_expr *)sm_new_double(0);
        }
    } else {
      printf("Could not find variable: %s\n", &(var_name->content));
      printf("In this expression, %s should be set to a context for this syntax to be valid.\n", &(var_name->content));
      $$= (sm_expr *)sm_new_double(0);
    }
  }
  | SYM CONTEXT  {
    sm_string    *var_name = ($1)->name;
    sm_search_result sr       = sm_find_var_index(sm_global_context(NULL), var_name);
    if (sr.found == true) {
      sm_object * found = sm_context_entries(sm_global_context(NULL))[sr.index].value;
      if(found->my_type==sm_meta_type){
        sm_meta* found_meta = (sm_meta*)found;
        ($2)->parent = sm_global_context($2);
        $$=(sm_expr*)sm_engine_eval(found_meta->address);
        sm_global_context(($2)->parent);
      }
    } else {
      printf("Could not find variable: %s\n", &(var_name->content));
      printf(", which should be set to a meta object for this syntax to be valid.\n");
      $$= (sm_expr *)sm_new_double(0);
    }
  }
 | SYM SYM  {
  sm_string    *var_name  =  ($1)->name;
  sm_string    *var_name2 = ($2)->name;
  sm_search_result sr        = sm_find_var_index(sm_global_context(NULL), var_name);
  sm_search_result sr2       = sm_find_var_index(sm_global_context(NULL), var_name2);
  if (sr.found == true && sr2.found == true) {
    sm_object * found = sm_context_entries(sm_global_context(NULL))[sr.index].value;
    if(found->my_type==sm_meta_type){
      sm_object * found2 = sm_context_entries(sm_global_context(NULL))[sr2.index].value;
      if(found2->my_type==sm_context_type){
        sm_meta * found_meta = (sm_meta*)found;
        sm_context *given_cx = (sm_context*)found2;
        given_cx->parent = sm_global_context(given_cx);
        $$=(sm_expr*)sm_engine_eval(found_meta->address);
        sm_global_context(given_cx->parent);
      }else{
        printf("Second variable is not a context: %s\n",&(var_name2->content));
        $$= (sm_expr *)sm_new_double(0);
      }
    } else {
      printf("First variable, \'%s\' needs to be a meta function.\n",&(var_name->content));
      $$= (sm_expr *)sm_new_double(0);
    }
  } else {
      if(sr.found==false)
        printf("Could not find variable: %s\n", &(var_name->content));
      if(sr2.found==false)
        printf("Could not find variable: %s\n", &(var_name2->content));
      printf("Two adjacent values are considered a function call.\n");
      printf("The variable \'%s\' must be set to a meta function.\nAlso, the variable ",&(var_name->content));
      printf("\'%s\' must be set to a context.\n",&(var_name2->content) );
      $$= (sm_expr *)sm_new_double(0);
    }
  }

META_EXPR : ':' EXPR {
        $$ = sm_new_meta((sm_object*) $2 ) ;
  }

ARRAY: ARRAY_LIST ']' { ; }

ARRAY_LIST: '[' EXPR ',' EXPR {$$=sm_new_expr_2(sm_array,(sm_object*)$2,(sm_object*)$4);}
  | ARRAY_LIST ',' EXPR {$$ = sm_append_to_expr($1,(sm_object*)$3);}

CONTEXT: CONTEXT_LIST ';' '}'     {;}
  | '{' KEYVALUE ';' '}'	{
    $$ = sm_set_var(sm_new_context(1),$2->name,$2->value);
  }
  | '{' '}'							{ $$ = sm_new_context(0);}

CONTEXT_LIST: '{' KEYVALUE ';' KEYVALUE {
	  $$ = sm_new_context(2);
	  $$ = sm_set_var(($$),$2->name,$2->value);
	  $$ = sm_set_var(($$),$4->name,$4->value);
  }
  | CONTEXT_LIST ';' KEYVALUE {
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
  
  //Initialize the current mem heap
  sm_global_current_heap(sm_new_heap(7500));

  //Initialize the global space arrays
  sm_global_space_array(sm_new_space_array(0,100));

  //Initialize the global context
  sm_global_context(sm_new_context(0));

  //Introduction and prompt
  printf("Symbolic Math System\n");
  printf("Version 0.11\n");
  sm_terminal_prompt();

  //Start the parser. 
  yyparse();

  //Exit gracefully.
  sm_signal_handler(SIGQUIT);
}
