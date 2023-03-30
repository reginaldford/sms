%{
  // The following file is provided under the BSD 2-clause license. For more info, read LICENSE.txt.

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include "../main/sms.h"

extern int yylex();
void       yyerror(char *msg);

void _lex_file(char *fpath);
void _done_lexing_file();
void _lex_cstr(char * cstr,int len);

%}

%union {
  sm_double        *num;
  sm_symbol        *sym;
  sm_expr          *expr;
  sm_string        *str;
  sm_context       *context;
  sm_meta          *meta;
  sm_fun           *fun;
  sm_expr          *param_list;
  sm_fun_param_obj *param;
};

%type <fun> FUN
%type <fun> FUN_INTRO
%type <expr> FUN_CALL
%type <expr> FUN_CALL_OPEN
%type <param_list> PARAM_LIST
%type <expr> PARAM_LIST_OPEN
%type <expr> IF_STATEMENT
%type <meta> META_EXPR
%type <expr> ASSIGNMENT
%type <context> CONTEXT
%type <context> CONTEXT_LIST
%type <expr> EXPR
%type <expr> EXPR_LIST
%type <expr> ARRAY
%type <expr> ARRAY_LIST
%type <expr> EQ
%type <expr> GT
%type <expr> LT
%type <expr> SEQUENCE
%type <expr> SEQUENCE_LIST
%type <expr> COMMAND

%token CLEAR FORMAT LS NEWLINE EXIT SELF IF

%token DOT
%token <fun> ARROW
%token <num> NUM
%token <expr> PIPE
%token <expr> EQEQ
%token <sym> SYM
%token <str> STRING
%token <expr> SIN
%token <expr> COS
%token <expr> TAN
%token <expr> SINH
%token <expr> COSH
%token <expr> TANH
%token <expr> SECH
%token <expr> CSCH
%token <expr> COTH
%token <expr> SEC
%token <expr> CSC
%token <expr> COT
%token <expr> LN
%token <expr> EXP
%token <expr> SQRT
%token <expr> RM
%token <expr> LET
%token <expr> ABS
%token <expr> DIFF
%token <expr> SIMP
%token <expr> MAP
%token <expr> PARENT
%token <expr> SIZE
%token <expr> WHILE
%token <expr> PRINT
%token <expr> TO_STRING
%token <expr> EVAL
%token <expr> PARSE
%token <expr> PARSE_FILE
%token <expr> READ_FILE
%token <expr> WRITE_FILE
%token <expr> RAND
%token <expr> ROUND
%token <expr> LT_EQ
%token <expr> GT_EQ
%token <expr> NOT
%token <expr> OR
%token <expr> INPUT
%token <expr> ESCAPE
%token DONE

%left ';'
%left '='
%left IF
%nonassoc '<' '>' EQEQ
%left '+' '-'
%left '*' '/'
%left '^'
%left ':'
%left DOT
%%


COMMAND : EXPR ';' {
  sm_global_parser_output((sm_object *)($1));
  YYACCEPT;
}
| RM SYM ';' {
  bool success = (sm_expr *)sm_context_rm(*(sm_global_lex_stack(NULL)->top), (sm_symbol *)$2);
  if (success == true) {
    sm_global_parser_output((sm_object *)sm_new_symbol(sm_new_string(4, "true")));
    YYACCEPT;
  } else {
    sm_global_parser_output((sm_object *)sm_new_symbol(sm_new_string(5, "false")));
    YYACCEPT;
  }
}
| error ';' { YYABORT; }
| DONE      { YYACCEPT;}



EXPR : SELF { $$ = (sm_expr *)*(sm_global_lex_stack(NULL)->top); }
| EXIT '(' EXPR ')'  { $$ = sm_new_expr(SM_EXIT_EXPR,(sm_object*)$3);  }
| LET SYM '=' EXPR { $$ = sm_new_expr_2(SM_LET_EXPR,(sm_object*)$2,(sm_object*)$4);}
| SYM{}
| EXPR '+' EXPR { $$ = sm_new_expr_2(SM_PLUS_EXPR, (sm_object *)$1, (sm_object *)$3); }
| EXPR '-' EXPR { $$ = sm_new_expr_2(SM_MINUS_EXPR, (sm_object *)$1, (sm_object *)$3); }
| EXPR '*' EXPR { $$ = sm_new_expr_2(SM_TIMES_EXPR, (sm_object *)$1, (sm_object *)$3); }
| EXPR '/' EXPR { $$ = sm_new_expr_2(SM_DIVIDE_EXPR, (sm_object *)$1, (sm_object *)$3); }
| EXPR '^' EXPR { $$ = sm_new_expr_2(SM_POW_EXPR, (sm_object *)$1, (sm_object *)$3); }
| NUM{}
| '-' EXPR {
  if (((sm_object *)$2)->my_type == SM_DOUBLE_TYPE) {
    ((sm_double *)$2)->value *= -1;
    $$ = (sm_expr *)$2;
  } else {
    $$ = sm_new_expr_2(SM_TIMES_EXPR, (sm_object *)sm_new_double(-1), (sm_object *)$2);
  }
}
| '(' EXPR ')' { $$ = (sm_expr *)$2; }
| STRING{}
| SIN '(' EXPR ')' { $$ = sm_new_expr(SM_SIN_EXPR, (sm_object *)$3); }
| COS '(' EXPR ')' { $$ = sm_new_expr(SM_COS_EXPR, (sm_object *)$3); }
| TAN '(' EXPR ')' { $$ = sm_new_expr(SM_TAN_EXPR, (sm_object *)$3); }
| SINH '(' EXPR ')' { $$ = sm_new_expr(SM_SINH_EXPR, (sm_object *)$3); }
| COSH '(' EXPR ')' { $$ = sm_new_expr(SM_COSH_EXPR, (sm_object *)$3); }
| TANH '(' EXPR ')' { $$ = sm_new_expr(SM_TANH_EXPR, (sm_object *)$3); }
| SEC '(' EXPR ')' { $$ = sm_new_expr(SM_SEC_EXPR, (sm_object *)$3); }
| CSC '(' EXPR ')' { $$ = sm_new_expr(SM_CSC_EXPR, (sm_object *)$3); }
| COT '(' EXPR ')' { $$ = sm_new_expr(SM_COT_EXPR, (sm_object *)$3); }
| SECH '(' EXPR ')' { $$ = sm_new_expr(SM_SECH_EXPR, (sm_object *)$3); }
| CSCH '(' EXPR ')' { $$ = sm_new_expr(SM_CSCH_EXPR, (sm_object *)$3); }
| COTH '(' EXPR ')' { $$ = sm_new_expr(SM_COTH_EXPR, (sm_object *)$3); }
| LN '(' EXPR ')' { $$ = sm_new_expr(SM_LN_EXPR, (sm_object *)$3); }
| EXP '(' EXPR ')' { $$ = sm_new_expr(SM_EXP_EXPR, (sm_object *)$3); }
| SQRT '(' EXPR ')' { $$ = sm_new_expr(SM_SQRT_EXPR, (sm_object *)$3); }
| ABS '(' EXPR ')' { $$ = sm_new_expr(SM_ABS_EXPR, (sm_object *)$3); }
| DIFF '(' EXPR ',' EXPR ')' { $$ = sm_new_expr_2(SM_DIFF_EXPR, (sm_object *)$3, (sm_object *)$5); }
| SIMP '(' EXPR ')' { $$ = sm_new_expr(SM_SIMP_EXPR, (sm_object *)$3); }
| MAP '(' EXPR ',' EXPR ')' { $$ = sm_new_expr_2(SM_MAP_EXPR,(sm_object*)$3,(sm_object*)$5); }
| EXPR_LIST ')' {}
| CONTEXT{}
| ARRAY{}
| META_EXPR{}
| ASSIGNMENT{}
| FUN_CALL{}
| FUN{}
| IF_STATEMENT{}
| EQ{}
| LT{}
| GT{}
| EXPR LT_EQ EXPR { $$ = sm_new_expr_2(SM_LT_EQ_EXPR,(sm_object*)$1,(sm_object*)$3);};
| EXPR GT_EQ EXPR { $$ = sm_new_expr_2(SM_GT_EQ_EXPR,(sm_object*)$1,(sm_object*)$3);};
| SEQUENCE {}
| SYM DOT SYM {$$ = sm_new_expr_2(SM_DOT_EXPR,(sm_object*)$1,(sm_object*)$3);}
| EXPR DOT SYM {$$ = sm_new_expr_2(SM_DOT_EXPR,(sm_object*)$1,(sm_object*)$3);}
| EXPR '[' EXPR ']' {$$ = sm_new_expr_2(SM_INDEX_EXPR,(sm_object*)$1,(sm_object*)$3);}
| PARENT '(' EXPR ')' {$$ = sm_new_expr(SM_PARENT_EXPR,(sm_object*)$3);}
| SIZE '(' EXPR ')' {$$ = sm_new_expr(SM_SIZE_EXPR,(sm_object*)$3);}
| WHILE '(' EXPR ',' EXPR ')' {$$ = sm_new_expr_2(SM_WHILE_EXPR,(sm_object*)$3,(sm_object*)$5);}
| PRINT '(' EXPR ')' {$$ = sm_new_expr(SM_PRINT_EXPR,(sm_object*)$3);}
| TO_STRING '(' EXPR ')' {$$ = sm_new_expr(SM_TO_STRING_EXPR,(sm_object*)$3);}
| EVAL '(' EXPR ')' {$$ = sm_new_expr(SM_EVAL_EXPR,(sm_object*)$3);}
| EVAL '(' EXPR ',' EXPR ')' {$$ = sm_new_expr_2(SM_EVAL_EXPR,(sm_object*)$3,(sm_object*)$5);}
| PARSE '(' EXPR ')' {$$ = sm_new_expr(SM_PARSE_EXPR,(sm_object*)$3);}
| PARSE_FILE '(' EXPR ')' {$$ = sm_new_expr(SM_PARSE_FILE_EXPR,(sm_object*)$3);}
| READ_FILE '(' EXPR ')' {$$ = sm_new_expr(SM_READ_FILE_EXPR,(sm_object*)$3);}
| WRITE_FILE '(' EXPR ',' EXPR ')' {
  $$ = sm_new_expr_2(SM_WRITE_FILE_EXPR,(sm_object*)$3,(sm_object*)$5);
  }
| RAND  '(' ')' { $$ = sm_new_expr_n(SM_RAND_EXPR,0,0);};
| ROUND '(' EXPR ')' { $$ = sm_new_expr(SM_ROUND_EXPR,(sm_object*)$3);};
| NOT   '(' EXPR ')' { $$ = sm_new_expr(SM_NOT_EXPR,(sm_object*)$3);};
| INPUT '(' ')' { $$ = sm_new_expr_n(SM_INPUT_EXPR,0,0);};
| EXPR OR EXPR   { $$ = sm_new_expr_2(SM_OR_EXPR,(sm_object*)$1,(sm_object*)$3);};
| ESCAPE '(' EXPR ')' { $$ = sm_new_expr(SM_ESCAPE_EXPR,(sm_object*)$3);};

FUN : FUN_INTRO EXPR {
  //local variables are changed from symbol to local
  $1->content = (sm_object *)sm_localize((sm_object*)$2, $1);
  $$ = $1;
}

FUN_INTRO : PARAM_LIST ARROW {
  sm_context * parent_cx = *(sm_global_lex_stack(NULL)->top);
  sm_fun *new_fun = sm_new_fun(parent_cx, ($1)->size, (sm_object *)NULL);
  sm_context_add_child(parent_cx, (sm_object*)new_fun);
  for (unsigned int i = 0; i < ($1)->size; i++) {
    sm_fun_param_obj *po = (sm_fun_param_obj *)sm_expr_get_arg($1, i);
    sm_fun_set_param(new_fun, i, po->name, po->default_val, po->known_expr);
  }
  $$ = new_fun;
}

PARAM_LIST : '(' ')' {
  $$ = sm_new_expr_n(SM_PARAM_LIST_EXPR, 0, 0);
}
| '(' SYM ')' {
  sm_fun_param_obj * fpo = sm_new_fun_param_obj($2->name,NULL,0);
  $$                = sm_new_expr(SM_PARAM_LIST_EXPR,(sm_object*)fpo);
}
| PARAM_LIST_OPEN ')' {}
| PARAM_LIST_OPEN SYM ')' {
  sm_fun_param_obj * new_param= sm_new_fun_param_obj($2->name,NULL,0);
  $$ = sm_expr_append($1, (sm_object *)new_param);
}

PARAM_LIST_OPEN : '(' SYM ',' {
  sm_expr *new_expr = sm_new_expr_n(SM_PARAM_LIST_EXPR, 1, 3);
  sm_fun_param_obj * new_param= sm_new_fun_param_obj($2->name,NULL,0);
  $$                = sm_expr_set_arg(new_expr, 0, (sm_object*)new_param);
}
| PARAM_LIST_OPEN SYM ',' {
  sm_fun_param_obj * new_param= sm_new_fun_param_obj($2->name,NULL,0);
  $$ = sm_expr_append($1, (sm_object *)new_param);
}

ASSIGNMENT : SYM '=' EXPR { $$ = sm_new_expr_2(SM_ASSIGN_EXPR, (sm_object *)($1), (sm_object *)($3)); }

SEQUENCE : SEQUENCE_LIST ')' {}

SEQUENCE_LIST : '(' EXPR ';' EXPR { $$ = sm_new_expr_2(SM_THEN_EXPR, (sm_object *)$2, (sm_object *)$4); }
| SEQUENCE_LIST ';' EXPR { $$ = sm_expr_append((sm_expr *)$1, (sm_object *)$3); }

EXPR_LIST : '+' '(' EXPR ',' EXPR { $$ = sm_new_expr_2(SM_PLUS_EXPR, (sm_object *)$3, (sm_object *)$5); }
| '-' '(' EXPR ',' EXPR { $$ = sm_new_expr_2(SM_MINUS_EXPR, (sm_object *)$3, (sm_object *)$5); }
| '*' '(' EXPR ',' EXPR { $$ = sm_new_expr_2(SM_TIMES_EXPR, (sm_object *)$3, (sm_object *)$5); }
| '/' '(' EXPR ',' EXPR { $$ = sm_new_expr_2(SM_DIVIDE_EXPR, (sm_object *)$3, (sm_object *)$5); }
| EXPR_LIST ',' EXPR { $$ = sm_expr_append((sm_expr *)$1, (sm_object *)$3); }

LT : '<' '(' EXPR ',' EXPR ')' {
  $$ = sm_new_expr_2(SM_TEST_LT_EXPR, (sm_object *)($3), (sm_object *)($5));
}
| EXPR '<' EXPR { $$ = sm_new_expr_2(SM_TEST_LT_EXPR, (sm_object *)($1), (sm_object *)($3)); }

GT : '>' '(' EXPR ',' EXPR ')' {
  $$ = sm_new_expr_2(SM_TEST_GT_EXPR, (sm_object *)($3), (sm_object *)($5));
}
| EXPR '>' EXPR { $$ = sm_new_expr_2(SM_TEST_GT_EXPR, (sm_object *)($1), (sm_object *)($3)); }

EQ : EQEQ '(' EXPR ',' EXPR ')' {
  $$ = sm_new_expr_2(SM_TEST_EQ_EXPR, (sm_object *)($3), (sm_object *)($5));
}
| EXPR EQEQ EXPR { $$ = sm_new_expr_2(SM_TEST_EQ_EXPR, (sm_object *)($1), (sm_object *)($3)); }

IF_STATEMENT : IF '(' EXPR ',' EXPR ')' {
  $$ = sm_new_expr_2(SM_IF_EXPR, (sm_object *)($3), (sm_object *)($5));
}
| IF '(' EXPR ',' EXPR ',' EXPR ')' {
  $$ = sm_new_expr_3(SM_IF_ELSE_EXPR, (sm_object *)($3), (sm_object *)($5), (sm_object *)($7));
}

FUN_CALL : FUN_CALL_OPEN ')' {}
| SYM '(' ')'{
  // store the function in the fun_call here
  // index: 0   1
  // value: fun args
  // if the function doesnt exist yet:
  // value: symbol args
  sm_string * var_name = $1->name;
  sm_object * found = sm_context_get_by_name_far(*(sm_global_lex_stack(NULL)->top),var_name);
  if(found==NULL){
    found=(sm_object*)$1;
  }
  sm_expr * args= sm_new_expr_n(SM_PARAM_LIST_EXPR,0,0);
  $$ = sm_new_expr_2(SM_FUN_CALL_EXPR, (sm_object *)found, (sm_object *)args);
}

FUN_CALL_OPEN : SYM '(' EXPR {
  // store the function in the fun_call here
  // index: 0   1
  // value: fun args
  // if the function doesnt exist yet:
  // value: symbol args
  sm_string * var_name = $1->name;
  sm_object * found = sm_context_get_by_name_far(*(sm_global_lex_stack(NULL)->top),var_name);
  if(found==NULL){
    found=(sm_object*)$1;
  }
  sm_expr * args= sm_new_expr_n(SM_PARAM_LIST_EXPR,1,2);
  args=sm_expr_set_arg(args,0,(sm_object*)$3);
  $$ = sm_new_expr_2(SM_FUN_CALL_EXPR, (sm_object *)found, (sm_object *)args);
}
| FUN_CALL_OPEN ',' EXPR {
  sm_expr * args = (sm_expr*)sm_expr_get_arg($1,1);
  args = sm_expr_append(args, (sm_object *)$3);
  $$=sm_expr_set_arg($1,1,(sm_object*)args);
}

META_EXPR : ':' EXPR { $$ = sm_new_meta((sm_object *)$2, *(sm_global_lex_stack(NULL))->top); }

ARRAY : ARRAY_LIST ']' {};
| ARRAY_LIST ',' ']' {};
| '[' EXPR ']' { $$ = (sm_expr *)sm_new_expr(SM_ARRAY_EXPR, (sm_object *)$2); }
| '[' ']' { $$ = sm_new_expr_n(SM_ARRAY_EXPR, 0, 0); }

ARRAY_LIST : '[' EXPR ',' EXPR { $$ = sm_new_expr_2(SM_ARRAY_EXPR, (sm_object *)$2, (sm_object *)$4); }
| ARRAY_LIST ',' EXPR { $$ = sm_expr_append($1, (sm_object *)$3); }

CONTEXT : CONTEXT_LIST '}' {
  $1->parent = *(sm_global_lex_stack(NULL)->top);
  sm_stack_pop(sm_global_lex_stack(NULL));
}
| CONTEXT_LIST ';' '}' {
  $1->parent = *(sm_global_lex_stack(NULL)->top);
  sm_stack_pop(sm_global_lex_stack(NULL));
}
| '{' ASSIGNMENT ';' '}' {
  sm_context *parent_cx         = *(sm_global_lex_stack(NULL)->top);
  sm_context *new_cx            = sm_new_context(1, 1, parent_cx);
  sm_string  *name              = ((sm_symbol *)sm_expr_get_arg($2, 0))->name;
  sm_object  *value             = (sm_object *)sm_expr_get_arg($2, 1);
  *(sm_context_entries(new_cx)) = (sm_context_entry){.name = name, .value = value};
  sm_context_add_child(parent_cx, (sm_object *)new_cx);
  $$ = new_cx;
}
| '{' ASSIGNMENT '}' {
  sm_context *parent_cx         = *(sm_global_lex_stack(NULL)->top);
  sm_context *new_cx            = sm_new_context(1, 1, parent_cx);
  sm_string  *name              = ((sm_symbol *)sm_expr_get_arg($2, 0))->name;
  sm_object  *value             = ((sm_object *)sm_expr_get_arg($2, 1));
  *(sm_context_entries(new_cx)) = (sm_context_entry){.name = name, .value = value};
  sm_context_add_child(parent_cx, (sm_object *)new_cx);
  $$ = new_cx;
}
| '{' '}' {
  sm_context *parent_cx = *(sm_global_lex_stack(NULL)->top);
  sm_context *new_cx    = sm_new_context(0, 0, parent_cx);
  sm_context_add_child(parent_cx, (sm_object *)new_cx);
  $$ = new_cx;
}

CONTEXT_LIST : '{' ASSIGNMENT ';' ASSIGNMENT {
  sm_context *parent_cx = *(sm_global_lex_stack(NULL)->top);
  sm_context *new_cx    = sm_new_context(2, 2, parent_cx);
  sm_context_add_child(parent_cx, (sm_object *)new_cx);
  sm_context_entry *arr    = sm_context_entries(new_cx);
  sm_string        *name   = ((sm_symbol *)sm_expr_get_arg($2, 0))->name;
  sm_string        *name2  = ((sm_symbol *)sm_expr_get_arg($4, 0))->name;
  sm_object        *value  = (sm_object *)sm_expr_get_arg($2, 1);
  sm_object        *value2 = (sm_object *)sm_expr_get_arg($4, 1);
  short int         toggle = strcmp(&(name->content), &(name2->content)) < 0 ? 0 : 1;
  arr[toggle % 2]          = (sm_context_entry){.name = name, .value = value};
  arr[(toggle + 1) % 2]    = (sm_context_entry){.name = name2, .value = value2};
  sm_global_lex_stack(sm_stack_push(sm_global_lex_stack(NULL), new_cx));
  $$ = new_cx;
}
| CONTEXT_LIST ';' ASSIGNMENT {
  sm_string  *name   = ((sm_symbol *)sm_expr_get_arg($3, 0))->name;
  sm_object  *value  = (sm_object *)sm_expr_get_arg($3, 1);
  sm_context *new_cx = sm_context_let($1, name, value);
  $$                 = new_cx;
}


%%


void lex_file(char *fpath){
  _lex_file(fpath);
}

void done_lexing_file(){
  _done_lexing_file();
}

void lex_cstr(char * cstr,int len){
  _lex_cstr(cstr,len);
}

void yyerror(char *msg) {
  // Use this function to investigate the error.
  fprintf(stderr, "Error: %s\n", msg);
}
