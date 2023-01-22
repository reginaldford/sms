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
  sm_meta * meta;
};

%token CLEAR FORMAT LS NEWLINE EXIT SELF IF

%type  <expr>     FUNCALL
%type  <expr>     IF_STATEMENT
%type  <meta>     META_EXPR
%type  <expr>     ASSIGNMENT
%type  <context>  CONTEXT
%type  <context>  CONTEXT_LIST
%type  <expr>     EXPR
%type  <expr>     EXPR_LIST
%type  <expr>     ARRAY
%type  <expr>     ARRAY_LIST
%type  <expr>     EQ
%type  <expr>     GT
%type  <expr>     LT
%token <num>      NUM
%token <expr>     EQEQ
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
%token <expr>     NOT
%token <expr>     ABS

%left  '+' '-'
%left  '*' '/'
%right '^'
%left  ':'



%%



COMMAND : EXPR {
    printf("%s\n",&(sm_object_to_string(sm_engine_eval((sm_object*)$1))->content));
    sm_garbage_collect();
    sm_terminal_prompt();
  }
  | DELETE SYM {
    sm_delete((sm_symbol*)$2);
    sm_garbage_collect();
    sm_terminal_prompt();
  }
  | LET SYM '=' EXPR { printf("Activated let command ! (incomplete) \n"); }
  | error { yyerror("Bad syntax.");}
  | COMMAND ';' COMMAND ';'
  | EXIT    ';' { sm_signal_handler(SIGQUIT); }



EXPR : SELF { $$ = (sm_expr*)sm_global_context(NULL); }
  | EXPR '+' EXPR { $$ = sm_new_expr_2(sm_plus,  (sm_object*) $1, (sm_object*) $3 ) ; }
  | EXPR '-' EXPR { $$ = sm_new_expr_2(sm_minus, (sm_object*) $1, (sm_object*) $3 ) ; }
  | EXPR '*' EXPR { $$ = sm_new_expr_2(sm_times, (sm_object*) $1, (sm_object*) $3 ) ; }
  | EXPR '/' EXPR { $$ = sm_new_expr_2(sm_divide,(sm_object*) $1, (sm_object*) $3 ) ; }
  | EXPR '^' EXPR { $$ = sm_new_expr_2(sm_pow,   (sm_object*) $1, (sm_object*) $3 ) ; }
  | NUM {}
  | '-' EXPR {
  if(((sm_object*)$2)->my_type == sm_double_type){
     ((sm_double*)$2)->value *= -1;
     $$ = (sm_expr*)$2;
    }
    else {
     $$ = sm_new_expr_2(sm_times,(sm_object*)sm_new_double(-1), (sm_object*)$2 );
    }
  }
  | SYM {}
  | '(' EXPR ')' { $$ = (sm_expr*) $2; }
  | STRING {}
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
  | EXPR_LIST ')' {}
  | CONTEXT {}
  | ARRAY {}
  | META_EXPR {}
  | ASSIGNMENT {}
  | FUNCALL {}
  | IF_STATEMENT {}
  | EQ {}
  | LT {}
  | GT {}

EXPR_LIST: '+' '('  EXPR ',' EXPR  {$$ = sm_new_expr_2(sm_plus,(sm_object*)$3,(sm_object*)$5 );}
  | '-' '('  EXPR ',' EXPR {$$ = sm_new_expr_2(sm_minus,(sm_object*)$3,(sm_object*)$5 );}
  | '*' '('  EXPR ',' EXPR {$$ = sm_new_expr_2(sm_times,(sm_object*)$3,(sm_object*)$5 );}
  | '/' '('  EXPR ',' EXPR {$$ = sm_new_expr_2(sm_divide,(sm_object*)$3,(sm_object*)$5 );}
  | EXPR_LIST ',' EXPR {$$ = sm_append_to_expr((sm_expr*)$1,(sm_object*)$3);}
  
LT : '<' '(' EXPR ',' EXPR ')' { $$ = sm_new_expr_2(sm_test_lt,(sm_object*)($3),(sm_object*)($5));}
  | EXPR '<' EXPR { $$ = sm_new_expr_2(sm_test_lt,(sm_object*)($1),(sm_object*)($3));}
  
GT : '>' '(' EXPR ',' EXPR ')' { $$ = sm_new_expr_2(sm_test_gt,(sm_object*)($3),(sm_object*)($5));}
  | EXPR '>' EXPR { $$ = sm_new_expr_2(sm_test_gt,(sm_object*)($1),(sm_object*)($3));}

EQ : EQEQ '(' EXPR ',' EXPR ')' { $$ = sm_new_expr_2(sm_test_eq,(sm_object*)($3),(sm_object*)($5));}
  | EXPR EQEQ EXPR { $$ = sm_new_expr_2(sm_test_eq,(sm_object*)($1),(sm_object*)($3));}

IF_STATEMENT : IF '(' EXPR ',' EXPR ')' {
    $$ = sm_new_expr_2(sm_if,(sm_object*)($3),(sm_object*)($5));
  }
  | IF '(' EXPR ',' EXPR ',' EXPR ')' {
    $$ = sm_new_expr_3(sm_if_else,(sm_object*)($3),(sm_object*)($5),(sm_object*)($7));
  }

ASSIGNMENT : SYM '=' EXPR {
    $$ = sm_new_expr_2(sm_assign,(sm_object*)($1),(sm_object*)($3));
  }

FUNCALL: META_EXPR CONTEXT {
    $$=sm_new_expr_2(sm_funcall_l_l, (sm_object *)($1), (sm_object *)($2));
  }
  | META_EXPR SYM {
    $$=sm_new_expr_2(sm_funcall_l_v, (sm_object *)($1), (sm_object *)($2));
  }
  | SYM CONTEXT  {
    $$=sm_new_expr_2(sm_funcall_v_l, (sm_object *)($1), (sm_object *)($2));
  }
  | SYM SYM  {
    $$=sm_new_expr_2(sm_funcall_v_v, (sm_object *)($1), (sm_object *)($2));
  }

META_EXPR : ':' EXPR {
        $$ = sm_new_meta((sm_object*) $2 ) ;
  }

ARRAY: ARRAY_LIST ']'
  | ARRAY_LIST ',' ']'
  | '[' EXPR ']' { $$ = (sm_expr*)sm_new_expr(sm_array,(sm_object*)$2);}
  | '[' ']'	{ $$ = sm_new_expr_n(sm_array,0,0);}

ARRAY_LIST: '[' EXPR ',' EXPR {$$=sm_new_expr_2(sm_array,(sm_object*)$2,(sm_object*)$4);}
  | ARRAY_LIST ',' EXPR {$$ = sm_append_to_expr($1,(sm_object*)$3);}

CONTEXT: CONTEXT_LIST '}' {}
  | '{' ASSIGNMENT ';' '}' {}
  | '{' ASSIGNMENT '}' {
    sm_string *name  = ((sm_symbol*)sm_get_expr_arg($2,0))->name;
    sm_object *value = (sm_object*)sm_get_expr_arg($2,1);
    $$ = sm_set_var(sm_new_context(1),name,value);
  }
  | '{' '}' { $$ = sm_new_context(0);}

CONTEXT_LIST: '{' ASSIGNMENT ';' ASSIGNMENT {
    sm_string *name  = ((sm_symbol*)sm_get_expr_arg($2,0))->name;
    sm_object *value = (sm_object*)sm_get_expr_arg($2,1);
	  $$ = sm_new_context(2);
	  $$ = sm_set_var(($$),name,value);
	  $$ = sm_set_var(($$),name,value);
  }
  | CONTEXT_LIST ';' ASSIGNMENT {
    sm_string *name  = ((sm_symbol*)sm_get_expr_arg($3,0))->name;
    sm_object *value = (sm_object*)sm_get_expr_arg($3,1);
    $$ = sm_set_var($1,name,value);
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
  printf("Version 0.125\n");
  sm_terminal_prompt();

  //Start the parser. 
  yyparse();

  //Exit gracefully.
  sm_signal_handler(SIGQUIT);
}
