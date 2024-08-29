%{
// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "../main/sms.h"

extern int yylex();
extern int yylineno;
void       yyerror(const char *msg);

FILE* _lex_file(char *fpath);
void _done_lexing_file(FILE* f);
void _lex_cstr(char * cstr,int len);
sm_cx *_note();
char parsing_fpath[256];
int parsing_fpath_len;

%}

%union {
  uint32_t         integer;
  sm_f64           *num;
  sm_ui8           *ui8;
  sm_symbol        *sym;
  sm_expr          *expr;
  sm_string        *str;
  sm_cx            *context;
  sm_meta          *meta;
  sm_fun           *fun;
  sm_error         *err;
  sm_expr          *param_list;
  sm_fun_param_obj *param;
  sm_array         *array;
};

%type <fun> FUN
%type <err> ERROR
%type <fun> FUN_INTRO
%type <expr> FUN_CALL
%type <expr> FUN_CALL_OPEN
%type <param_list> PARAM_LIST
%type <array> F64_ARRAY
%type <array> F64_ARRAY_OPEN
%type <array> F64_ARRAY_LIST
%type <array> UI8_ARRAY
%type <array> UI8_ARRAY_OPEN
%type <array> UI8_ARRAY_LIST
%type <expr> PARAM_LIST_OPEN
%type <expr> IF_STATEMENT
%type <meta> META_EXPR
%type <expr> ASSIGNMENT
%type <expr> INDEX_ASSIGNMENT
%type <expr> DOT_ASSIGNMENT
%type <context> CONTEXT
%type <context> CONTEXT_LIST
%type <expr> EXPR
%type <expr> TUPLE
%type <expr> TUPLE_LIST
%type <expr> EQ
%type <expr> ASSOCIATION
%type <expr> TEST_LT
%type <expr> TEST_GT
%type <expr> TEST_LT_EQ
%type <expr> TEST_GT_EQ
%type <expr> TEST_ISNAN
%type <expr> TEST_ISINF
%type <expr> BLOCK
%type <expr> OPEN_BLOCK
%type <expr> COMMAND

%token SELF
%token ARGS
%token LS
%token CD
%token PWD
%token CLEAR

%token EXIT
%token VERSION
%token HELP

%token IS
%token DOT
%token <expr> F64
%token <expr> UI8
%token <integer> INTEGER
%token <expr> IPLUS
%token <expr> IMINUS
%token <expr> ITIMES
%token <expr> IDIVIDE
%token <expr> IPOW
%token <expr> IXOR
%token <expr> IAND
%token <expr> IOR
%token <expr> PLUSEQ
%token <expr> MINUSEQ
%token <expr> TIMESEQ
%token <expr> DIVIDEEQ
%token <expr> POWEREQ
%token <expr> INC 
%token <expr> DEC
%token <expr> PIPE
%token <expr> EQEQ
%token <expr> ASSOCIATE
%token <sym> SYM
%token <str> STRING
%token <expr> SIN
%token <expr> COS
%token <expr> TAN
%token <expr> ASIN
%token <expr> ACOS
%token <expr> ATAN
%token <expr> SINH
%token <expr> COSH
%token <expr> TANH
%token <expr> ASINH
%token <expr> ACOSH
%token <expr> ATANH
%token <expr> SECH
%token <expr> CSCH
%token <expr> COTH
%token <expr> ASECH
%token <expr> ACSCH
%token <expr> ACOTH
%token <expr> SEC
%token <expr> CSC
%token <expr> COT
%token <expr> ASEC
%token <expr> ACSC
%token <expr> ACOT
%token <expr> LN
%token <expr> LOG
%token <expr> EXP
%token <expr> SQRT
%token <expr> ABS

%token <expr> RM

%token F64_ARRAY_OPEN
%token UI8_ARRAY_OPEN

%token <expr> LET

%token <expr> DIFF
%token <expr> SIMP
%token <expr> INT

%token IF
%token ELSE
%token <expr> WHILE
%token <expr> FOR
%token <expr> DO_WHILE
%token <expr> RETURN
%token <expr> PARENT
%token <expr> EVAL
%token <expr> CX_EVAL
%token <expr> EVAL_FAST
%token <expr> EVAL_FAST_IN_CX
%token <expr> FAILS
%token <expr> PARSE
%token <expr> RUNTIME_META

%token <expr> NEW_ARR
%token <expr> MAP
%token <expr> REDUCE
%token <expr> SIZE
%token <expr> ZEROS
%token <expr> PART
%token <expr> REPEAT
%token <expr> CAT

%token <expr> STR_SIZE
%token <expr> STR_MUT
%token <expr> STR_CAT
%token <expr> STR_REPEAT
%token <expr> STR_GET
%token <expr> STR_SET
%token <expr> STR_MAP
%token <expr> STR_FIND
%token <expr> STR_FINDR
%token <expr> STR_SPLIT
%token <expr> STR_PART
%token <expr> STR_UNESCAPE
%token <expr> STR_ESCAPE
%token <expr> STR_SETCHAR
%token <expr> STR_GETCHAR
%token <expr> STR_TONUMS
%token <expr> STR_CMP
%token <expr> NEW_STR
%token <expr> TO_STRFMT

%token <expr> NEW_MEM
%token <expr> MEM_MAP
%token <expr> MEM_UNITE
%token <expr> MEM_PART
%token <expr> MEM_TONUMS
%token <expr> NUMS_TOMEM

%token <expr> PUT
%token <expr> PUTLN
%token <expr> INPUT

%token <expr> FILE_PARSE
%token <expr> FILE_READ
%token <expr> FILE_RUN
%token <expr> FILE_PART
%token <expr> FILE_WRITESTR
%token <expr> FILE_WRITETGA
%token <expr> FILE_EXISTS
%token <expr> FILE_STAT
%token <expr> FILE_APPEND
%token <expr> FILE_TOMEM
%token <expr> FILE_CP
%token <expr> FILE_MV
%token <expr> FILE_RM
%token <expr> FILE_SIZE
%token <expr> CSV_TOFILE
%token <expr> CSV_TOARR
%token <expr> FILE_TOCSV

%token <expr> RANDOM
%token <expr> SEED
%token <expr> ROUND
%token <expr> FLOOR
%token <expr> CEIL
%token <expr> MOD
%token <expr> LT
%token <expr> GT
%token <expr> LT_EQ
%token <expr> GT_EQ
%token <expr> ISNAN
%token <expr> ISINF

%token <expr> NOTEQ
%token <expr> NOT
%token <expr> OR
%token <expr> AND
%token <expr> XOR

%token <expr> NEW_CX
%token <expr> CX_SETPARENT
%token <expr> CX_KEYS
%token <expr> CX_VALUES
%token <expr> CX_LET
%token <expr> CX_SET
%token <expr> CX_GET
%token <expr> CX_HAS
%token <expr> CX_GET_FAR
%token <expr> CX_HAS_FAR
%token <expr> CX_CLEAR
%token <expr> CX_IMPORT
%token <expr> CX_MAP
%token <expr> CX_REDUCE
%token <expr> CX_CONTAINING
%token <expr> CX_SIZE
%token <expr> CX_RM

%token <fun>  ARROW
%token <expr> FN_XP
%token <expr> FN_SETXP
%token <expr> FN_PARAMS
%token <expr> FN_SETPARAMS
%token <expr> FN_PARENT
%token <expr> FN_SETPARENT

%token <expr> NEW_XP
%token <expr> XP_OP
%token <expr> XP_SETOP
%token <expr> XP_OPSYM

%token <expr> COPY
%token <expr> DEEP_COPY

%token <expr> SYM_NAME

%token <expr> MEM_SAVEFILE
%token <expr> MEM_READFILE
%token <expr> MEM_CHECKFILE


%token <expr> IS_ERR
%token <expr> ERR_TITLE
%token <expr> ERR_MESSAGE
%token <expr> ERR_SOURCE
%token <expr> ERR_LINE
%token <expr> ERR_NOTES
%token <expr> ERR_CASE

%token <expr> DATE_STR
%token <expr> DATE
%token <expr> TIME
%token <expr> SLEEP
%token <expr> OS_EXEC
%token <expr> OS_EXECTOSTR
%token <expr> OS_GETENV
%token <expr> OS_SETENV
%token <expr> FORK
%token <expr> WAIT
%token <expr> GETOPTIONS
%token <expr> SETOPTIONS
%token <expr> RESETOPTIONS
%token <expr> GC
%token <expr> THISPROCESS

%token DONE
%token ENDOFFILE

%nonassoc SYM
%nonassoc '<' '>' LT_EQ GT_EQ EQEQ IS ISNAN ISINF
%left ':'
%left  '='
%nonassoc ','
%left IF WHILE FOR DOWHILE
%left OR AND NOT
%left DOT '+' '-'
%left '*' '/'
%left '^'
%left ';'

%%

COMMAND : EXPR ';' {
  sm_global_parser_output((sm_object *)($1));
  YYACCEPT;
}
| error     { YYABORT; return 4; }
| DONE      { YYACCEPT; }
| ENDOFFILE { YYACCEPT; }

EXPR : SELF { $$ = (sm_expr*)sm_new_self(); }
| ARGS '(' ')' { $$ = sm_new_expr_0(SM_ARGS_EXPR, _note());}
| INPUT { $$ = (sm_expr*)sm_new_self(); }
| EXIT '(' EXPR ')' { $$ = sm_new_expr(SM_EXIT_EXPR,(sm_object*)$3, _note()); }
| EXIT '(' ')' { $$ = sm_new_expr_0(SM_EXIT_EXPR, _note()); }
| VERSION '(' ')' { $$ = sm_new_expr_0(SM_VERSION_EXPR, _note()); }
| LET SYM '=' EXPR { $$ = sm_new_expr_2(SM_LET_EXPR,(sm_object*)$2,(sm_object*)$4, _note()); }
| RM SYM {$$ = sm_new_expr(SM_RM_EXPR, (sm_object *)$2, _note()); }
| SYM{}
| SYM DOT SYM {$$ = sm_new_expr_2(SM_DOT_EXPR,(sm_object*)$1,(sm_object*)$3, _note()); }
| EXPR DOT SYM {$$ = sm_new_expr_2(SM_DOT_EXPR,(sm_object*)$1,(sm_object*)$3, _note()); }
| EXPR '+' EXPR { $$ = sm_new_expr_2(SM_PLUS_EXPR, (sm_object *)$1, (sm_object *)$3, _note()); }
| EXPR '-' EXPR { $$ = sm_new_expr_2(SM_MINUS_EXPR, (sm_object *)$1, (sm_object *)$3, _note()); }
| EXPR '*' EXPR { $$ = sm_new_expr_2(SM_TIMES_EXPR, (sm_object *)$1, (sm_object *)$3, _note()); }
| EXPR '/' EXPR { $$ = sm_new_expr_2(SM_DIVIDE_EXPR, (sm_object *)$1, (sm_object *)$3, _note()); }
| EXPR '^' EXPR { $$ = sm_new_expr_2(SM_POW_EXPR, (sm_object *)$1, (sm_object *)$3, _note()); }
| EXPR IPLUS EXPR { $$ = sm_new_expr_2(SM_IPLUS_EXPR, (sm_object *)$1, (sm_object *)$3, _note()); }
| EXPR IMINUS EXPR { $$ = sm_new_expr_2(SM_IMINUS_EXPR, (sm_object *)$1, (sm_object *)$3, _note()); }
| EXPR ITIMES EXPR { $$ = sm_new_expr_2(SM_ITIMES_EXPR, (sm_object *)$1, (sm_object *)$3, _note()); }
| EXPR IDIVIDE EXPR { $$ = sm_new_expr_2(SM_IDIVIDE_EXPR, (sm_object *)$1, (sm_object *)$3, _note()); }
| EXPR IPOW EXPR { $$ = sm_new_expr_2(SM_IPOW_EXPR, (sm_object *)$1, (sm_object *)$3, _note()); }
| EXPR IXOR EXPR { $$ = sm_new_expr_2(SM_IXOR_EXPR, (sm_object *)$1, (sm_object *)$3, _note()); }
| EXPR IAND EXPR { $$ = sm_new_expr_2(SM_IAND_EXPR, (sm_object *)$1, (sm_object *)$3, _note()); }
| EXPR IOR EXPR { $$ = sm_new_expr_2(SM_IOR_EXPR, (sm_object *)$1, (sm_object *)$3, _note()); }
| SYM PLUSEQ EXPR { $$ = sm_new_expr_2(SM_PLUSEQ_EXPR, (sm_object *)$1, (sm_object *)$3, _note()); }
| SYM MINUSEQ EXPR { $$ = sm_new_expr_2(SM_MINUSEQ_EXPR, (sm_object *)$1, (sm_object *)$3, _note()); }
| SYM TIMESEQ EXPR { $$ = sm_new_expr_2(SM_TIMESEQ_EXPR, (sm_object *)$1, (sm_object *)$3, _note()); }
| SYM DIVIDEEQ EXPR { $$ = sm_new_expr_2(SM_DIVIDEEQ_EXPR, (sm_object *)$1, (sm_object *)$3, _note()); }
| SYM POWEREQ EXPR { $$ = sm_new_expr_2(SM_POWEREQ_EXPR, (sm_object *)$1, (sm_object *)$3, _note()); }
| F64{}
| UI8{}
| INTEGER { $$ = (sm_expr*)sm_new_f64($1);}
| SYM INC { $$ = sm_new_expr(SM_INC_EXPR,(sm_object*)$1,_note()); }
| SYM DEC { $$ = sm_new_expr(SM_DEC_EXPR,(sm_object*)$1,_note()); }
| '-' EXPR {
  if (((sm_object *)$2)->my_type == SM_F64_TYPE) {
    ((sm_f64 *)$2)->value *= -1;
    $$ = $2;
  } else {
    $$ = sm_new_expr_2(SM_TIMES_EXPR, (sm_object *)sm_new_f64(-1), (sm_object *)$2, _note());
  }
}
| '(' EXPR ')' { $$ = $2; }
| STRING{}
| NEW_STR '(' EXPR ')' { $$ = sm_new_expr(SM_NEW_STR_EXPR, (sm_object *)$3, _note()); }
| SIN '(' EXPR ')' { $$ = sm_new_expr(SM_SIN_EXPR, (sm_object *)$3, _note()); }
| COS '(' EXPR ')' { $$ = sm_new_expr(SM_COS_EXPR, (sm_object *)$3, _note()); }
| TAN '(' EXPR ')' { $$ = sm_new_expr(SM_TAN_EXPR, (sm_object *)$3, _note()); }
| ASIN '(' EXPR ')' { $$ = sm_new_expr(SM_ASIN_EXPR, (sm_object *)$3, _note()); }
| ACOS '(' EXPR ')' { $$ = sm_new_expr(SM_ACOS_EXPR, (sm_object *)$3, _note()); }
| ATAN '(' EXPR ')' { $$ = sm_new_expr(SM_ATAN_EXPR, (sm_object *)$3, _note()); }
| SINH '(' EXPR ')' { $$ = sm_new_expr(SM_SINH_EXPR, (sm_object *)$3, _note()); }
| COSH '(' EXPR ')' { $$ = sm_new_expr(SM_COSH_EXPR, (sm_object *)$3, _note()); }
| TANH '(' EXPR ')' { $$ = sm_new_expr(SM_TANH_EXPR, (sm_object *)$3, _note()); }
| ASINH '(' EXPR ')' { $$ = sm_new_expr(SM_ASINH_EXPR, (sm_object *)$3, _note()); }
| ACOSH '(' EXPR ')' { $$ = sm_new_expr(SM_ACOSH_EXPR, (sm_object *)$3, _note()); }
| ATANH '(' EXPR ')' { $$ = sm_new_expr(SM_ATANH_EXPR, (sm_object *)$3, _note()); }
| SEC '(' EXPR ')' { $$ = sm_new_expr(SM_SEC_EXPR, (sm_object *)$3, _note()); }
| CSC '(' EXPR ')' { $$ = sm_new_expr(SM_CSC_EXPR, (sm_object *)$3, _note()); }
| COT '(' EXPR ')' { $$ = sm_new_expr(SM_COT_EXPR, (sm_object *)$3, _note()); }
| SECH '(' EXPR ')' { $$ = sm_new_expr(SM_SECH_EXPR, (sm_object *)$3, _note()); }
| CSCH '(' EXPR ')' { $$ = sm_new_expr(SM_CSCH_EXPR, (sm_object *)$3, _note()); }
| COTH '(' EXPR ')' { $$ = sm_new_expr(SM_COTH_EXPR, (sm_object *)$3, _note()); }
| LN '(' EXPR ')' { $$ = sm_new_expr(SM_LN_EXPR, (sm_object *)$3, _note()); }
| LOG '(' EXPR ',' EXPR ')' { $$ = sm_new_expr_2(SM_LOG_EXPR,(sm_object*)$3, (sm_object*)$5, _note());}
| EXP '(' EXPR ')' { $$ = sm_new_expr(SM_EXP_EXPR, (sm_object *)$3, _note()); }
| SQRT '(' EXPR ')' { $$ = sm_new_expr(SM_SQRT_EXPR, (sm_object *)$3, _note()); }
| ABS '(' EXPR ')' { $$ = sm_new_expr(SM_ABS_EXPR, (sm_object *)$3, _note()); }
| DIFF '(' EXPR ',' EXPR ')' { $$ = sm_new_expr_2(SM_DIFF_EXPR, (sm_object *)$3, (sm_object *)$5, _note()); }
| SIMP '(' EXPR ')' { $$ = sm_new_expr(SM_SIMP_EXPR, (sm_object *)$3, _note()); }
| MAP '(' EXPR ',' EXPR ')' { $$ = sm_new_expr_2(SM_MAP_EXPR,(sm_object*)$3,(sm_object*)$5, _note()); }
| REDUCE '(' EXPR ',' EXPR ',' EXPR  ')' { $$ = sm_new_expr_3(SM_REDUCE_EXPR,(sm_object*)$3,(sm_object*)$5,(sm_object*)$7, _note()); }
| CONTEXT{}
| TUPLE{}
| SIZE '(' EXPR ')' {$$ = sm_new_expr(SM_SIZE_EXPR, (sm_object*)$3, _note());}
| ZEROS '(' EXPR ')' {$$ = sm_new_expr(SM_ZEROS_EXPR, (sm_object*)$3, _note());}
| PART '(' EXPR ',' EXPR ',' EXPR ')' {$$ = sm_new_expr_3(SM_PART_EXPR, (sm_object*)$3, (sm_object*)$5,(sm_object*)$7, _note());}
| REPEAT '(' EXPR ',' EXPR ')' {$$ = sm_new_expr_2(SM_REPEAT_EXPR, (sm_object*)$3, (sm_object*)$5, _note());}
| CAT '(' EXPR ',' EXPR ')' {$$ = sm_new_expr_2(SM_CAT_EXPR, (sm_object*)$3, (sm_object*)$5, _note());}
| META_EXPR{}
| ASSIGNMENT{}
| INDEX_ASSIGNMENT{}
| DOT_ASSIGNMENT{}
| ERROR {}
| FUN_CALL{}
| FUN{}
| IF_STATEMENT{}
| EQ{}
| EXPR IS EXPR { $$ = sm_new_expr_2(SM_IS_EXPR,(sm_object*)$1,(sm_object*)$3, _note());}
| TEST_LT{}
| TEST_GT{}
| TEST_LT_EQ{}
| TEST_GT_EQ{}
| TEST_ISNAN{}
| TEST_ISINF{}
| BLOCK {}
| EXPR '[' EXPR ']' {$$ = sm_new_expr_2(SM_INDEX_EXPR,(sm_object*)$1,(sm_object*)$3, _note());}
| SYM '[' EXPR ']' {$$ = sm_new_expr_2(SM_INDEX_EXPR,(sm_object*)$1,(sm_object*)$3, _note());}
| PARENT '(' EXPR ')' {$$ = sm_new_expr(SM_PARENT_EXPR,(sm_object*)$3, _note());}
| WHILE '(' EXPR ')' EXPR {$$ = sm_new_expr_2(SM_WHILE_EXPR,(sm_object*)$3,(sm_object*)$5, _note());}
| FOR '(' EXPR ';' EXPR ';' EXPR ')' EXPR { $$ = sm_new_expr_4(SM_FOR_EXPR,(sm_object*)$3,(sm_object*)$5,(sm_object*)$7,(sm_object*)$9,_note());}
| DO_WHILE '(' EXPR ')' EXPR {$$ = sm_new_expr_2(SM_DO_WHILE_EXPR,(sm_object*)$5,(sm_object*)$3, _note());}
| RETURN EXPR {$$ = sm_new_expr(SM_RETURN_EXPR,(sm_object*)$2, _note());}
| EVAL '(' EXPR ')' {$$ = sm_new_expr(SM_EVAL_EXPR,(sm_object*)$3, _note());}
| CX_EVAL '(' EXPR ',' EXPR ')' {$$ = sm_new_expr_2(SM_CX_EVAL_EXPR,(sm_object*)$3,(sm_object*)$5, _note());}
| EVAL_FAST '(' EXPR ')' {$$ = sm_new_expr(SM_EVAL_FAST_EXPR,(sm_object*)$3, _note());}
| EVAL_FAST_IN_CX '(' EXPR ',' EXPR ')' {$$ = sm_new_expr_2(SM_EVAL_FAST_EXPR,(sm_object*)$3,(sm_object*)$5, _note());}
| FAILS '(' EXPR ')' {$$ = sm_new_expr(SM_FAILS_EXPR,(sm_object*)$3, _note());}
| PARSE '(' EXPR ')' {$$ = sm_new_expr(SM_PARSE_EXPR,(sm_object*)$3, _note());}
| PUT '(' EXPR ')' {$$ = sm_new_expr(SM_PUT_EXPR,(sm_object*)$3, _note());}
| PUTLN '(' EXPR ')' {$$ = sm_new_expr(SM_PUTLN_EXPR,(sm_object*)$3, _note());}
| INPUT '(' ')' { $$ = sm_new_expr_n(SM_INPUT_EXPR,0,0, _note());}
| CX_SETPARENT '(' EXPR ','  EXPR ')' {$$ = sm_new_expr_2(SM_CX_SETPARENT_EXPR,(sm_object *)$3,(sm_object*)$5, _note());}
| CX_CONTAINING '(' EXPR ',' EXPR ')' {$$=sm_new_expr_2(SM_CX_CONTAINING_EXPR, (sm_object *)$3,(sm_object*)$5, _note());}
| CX_CLEAR '(' EXPR ')' {$$=sm_new_expr(SM_CX_CLEAR_EXPR, (sm_object *)$3, _note());}
| CX_IMPORT '(' EXPR ',' EXPR ')' {$$=sm_new_expr_2(SM_CX_IMPORT_EXPR, (sm_object *)$3, (sm_object*) $5, _note());}
| CX_LET '(' EXPR ',' EXPR ',' EXPR ')' { $$ = sm_new_expr_3(SM_CX_LET_EXPR,(sm_object*)$3,(sm_object*)$5, (sm_object*)$7, _note());}
| CX_SET '(' EXPR ',' EXPR ',' EXPR ')' { $$ = sm_new_expr_3(SM_CX_SET_EXPR,(sm_object*)$3,(sm_object*)$5, (sm_object*)$7, _note());}
| CX_GET '(' EXPR ',' EXPR ')' { $$ = sm_new_expr_2(SM_CX_GET_EXPR,(sm_object*)$3,(sm_object*)$5, _note());}
| CX_HAS '(' EXPR ',' EXPR ')' { $$ = sm_new_expr_2(SM_CX_HAS_EXPR,(sm_object*)$3,(sm_object*)$5, _note());}
| CX_GET_FAR '(' EXPR ',' EXPR ')' { $$ = sm_new_expr_2(SM_CX_GET_FAR_EXPR,(sm_object*)$3,(sm_object*)$5, _note());}
| CX_HAS_FAR '(' EXPR ',' EXPR ')' { $$ = sm_new_expr_2(SM_CX_HAS_FAR_EXPR,(sm_object*)$3,(sm_object*)$5, _note());}
| CX_RM '(' EXPR ',' EXPR ')' {$$ = sm_new_expr_2(SM_CX_RM_EXPR, (sm_object *)$3,(sm_object*)$5, _note());}
| CX_SIZE '(' EXPR ')' {$$ = sm_new_expr(SM_CX_SIZE_EXPR, (sm_object *)$3, _note());}
| CX_KEYS '(' EXPR ')' {$$ = sm_new_expr(SM_CX_KEYS_EXPR, (sm_object *)$3, _note());}
| CX_VALUES '(' EXPR ')' {$$ = sm_new_expr(SM_CX_VALUES_EXPR, (sm_object *)$3, _note());}
| FN_XP '(' EXPR ')' {$$ = sm_new_expr(SM_FN_XP_EXPR,(sm_object *)$3, _note());}
| FN_SETXP '(' EXPR ',' EXPR ')' {$$ = sm_new_expr_2(SM_FN_SETXP_EXPR,(sm_object *)$3,(sm_object*)$5, _note());}
| FN_PARAMS '(' EXPR ')' {$$ = sm_new_expr(SM_FN_PARAMS_EXPR,(sm_object *)$3, _note());}
| FN_SETPARAMS '(' EXPR ','  EXPR ')' {$$ = sm_new_expr_2(SM_FN_SETPARAMS_EXPR,(sm_object *)$3,(sm_object*)$5, _note());}
| FN_PARENT '(' EXPR ')' {$$ = sm_new_expr(SM_FN_PARENT_EXPR,(sm_object *)$3, _note());}
| FN_SETPARENT '(' EXPR ','  EXPR ')' {$$ = sm_new_expr_2(SM_FN_SETPARENT_EXPR,(sm_object *)$3,(sm_object*)$5, _note());}
| XP_OP '(' EXPR ')' {$$ = sm_new_expr(SM_XP_OP_EXPR,(sm_object*)$3, _note());}
| XP_SETOP '(' EXPR ',' EXPR ')' {$$ = sm_new_expr_2(SM_XP_SET_OP_EXPR,(sm_object*)$3,(sm_object*)$5, _note());}
| XP_OPSYM '(' EXPR ')' {$$ = sm_new_expr(SM_XP_OP_SYM_EXPR,(sm_object*)$3, _note());}
| F64_ARRAY { }
| UI8_ARRAY { }
| FILE_PARSE '(' EXPR ')' {$$ = sm_new_expr(SM_FILE_PARSE_EXPR,(sm_object*)$3, _note());}
| FILE_READ '(' EXPR ')' {$$ = sm_new_expr(SM_FILE_READSTR_EXPR,(sm_object*)$3, _note());}
| FILE_RUN '(' EXPR ',' EXPR ')' {$$ = sm_new_expr_2(SM_FILE_READSTR_EXPR,(sm_object*)$3,(sm_object*)$5, _note());}
| FILE_PART '(' EXPR ',' EXPR ',' EXPR ')' {$$ = sm_new_expr_3(SM_FILE_PART_EXPR,(sm_object*)$3,(sm_object*)$5,(sm_object*)$7, _note());}
| FILE_WRITESTR '(' EXPR ',' EXPR ')' {$$ = sm_new_expr_2(SM_FILE_WRITESTR_EXPR,(sm_object*)$3,(sm_object*)$5, _note());}
| FILE_WRITETGA '(' EXPR ',' EXPR ',' EXPR ',' EXPR ')' {$$ = sm_new_expr_4(SM_FILE_WRITETGA_EXPR,(sm_object*)$3,(sm_object*)$5,(sm_object*)$7,(sm_object*)$9, _note());}
| FILE_APPEND '(' EXPR ',' EXPR ')' {$$ = sm_new_expr_2(SM_FILE_APPEND_EXPR,(sm_object*)$3,(sm_object*)$5, _note());}
| FILE_EXISTS '(' EXPR ')' {$$ = sm_new_expr(SM_FILE_EXISTS_EXPR,(sm_object*)$3, _note());}
| FILE_RM '(' EXPR ')' {$$ = sm_new_expr(SM_FILE_RM_EXPR,(sm_object*)$3, _note());}
| FILE_STAT '(' EXPR ')' {$$ = sm_new_expr(SM_FILE_STAT_EXPR,(sm_object*)$3, _note());}
| RANDOM  '(' ')' { $$ = sm_new_expr_n(SM_RANDOM_EXPR,0,0, _note());}
| SEED  '(' EXPR ')' { $$ = sm_new_expr(SM_SEED_EXPR,(sm_object*)$3, _note());}
| ROUND '(' EXPR ')' { $$ = sm_new_expr(SM_ROUND_EXPR,(sm_object*)$3, _note());}
| CEIL '(' EXPR ')' { $$ = sm_new_expr(SM_CEIL_EXPR,(sm_object*)$3, _note());}
| MOD '(' EXPR ',' EXPR ')' { $$ = sm_new_expr_2(SM_MOD_EXPR,(sm_object*)$3,(sm_object*)$5, _note());}
| FLOOR '(' EXPR ')' { $$ = sm_new_expr(SM_FLOOR_EXPR,(sm_object*)$3, _note());}
| NOT   '(' EXPR ')' { $$ = sm_new_expr(SM_NOT_EXPR,(sm_object*)$3, _note());}
| EXPR OR EXPR   { $$ = sm_new_expr_2(SM_OR_EXPR,(sm_object*)$1,(sm_object*)$3, _note());}
| EXPR AND EXPR   { $$ = sm_new_expr_2(SM_AND_EXPR,(sm_object*)$1,(sm_object*)$3, _note());}
| STR_ESCAPE '(' EXPR ')' { $$ = sm_new_expr(SM_STR_ESCAPE_EXPR,(sm_object*)$3, _note());}
| STR_SIZE '(' EXPR ')' {$$ = sm_new_expr(SM_STR_SIZE_EXPR,(sm_object*)$3, _note());}
| STR_MUT '(' EXPR ',' EXPR ',' EXPR ')' {$$ = sm_new_expr_3(SM_STR_MUT_EXPR,(sm_object*)$3,(sm_object*)$5,(sm_object*)$7,    _note());}
| STR_FIND '(' EXPR ',' EXPR ')' {$$ = sm_new_expr_2(SM_STR_FIND_EXPR,(sm_object*)$3,(sm_object*)$5, _note());}
| STR_FINDR '(' EXPR ',' EXPR ')' {$$ = sm_new_expr_2(SM_STR_FINDR_EXPR,(sm_object*)$3,(sm_object*)$5, _note());}
| STR_CAT '(' EXPR ',' EXPR ')' {$$ = sm_new_expr_2(SM_STR_CAT_EXPR,(sm_object*)$3,(sm_object*)$5, _note());}
| EXPR STR_CAT EXPR {$$ = sm_new_expr_2(SM_STR_CAT_EXPR,(sm_object*)$1,(sm_object*)$3, _note());}
| STR_REPEAT '(' EXPR ',' EXPR ')' {$$ = sm_new_expr_2(SM_STR_REPEAT_EXPR,(sm_object*)$3,(sm_object*)$5, _note());}
| STR_SPLIT '(' EXPR ',' EXPR ')' {$$ = sm_new_expr_2(SM_STR_SPLIT_EXPR,(sm_object*)$3,(sm_object*)$5, _note());}
| STR_PART '(' EXPR ',' EXPR ',' EXPR ')' {$$ = sm_new_expr_3(SM_STR_PART_EXPR,(sm_object*)$3,(sm_object*)$5,(sm_object*)$7, _note());}
| DATE_STR '(' ')' { $$ = sm_new_expr_n(SM_DATE_STR_EXPR,0,0, _note());}
| DATE '(' ')' { $$ = sm_new_expr_n(SM_DATE_EXPR,0,0, _note());}
| TIME '(' ')' { $$ = sm_new_expr_n(SM_TIME_EXPR,0,0, _note());}
| SLEEP '(' EXPR ')' { $$ = sm_new_expr(SM_SLEEP_EXPR,(sm_object*)$3, _note());}
| OS_EXEC '(' EXPR ')' { $$ = sm_new_expr(SM_EXEC_EXPR,(sm_object*)$3, _note());}
| OS_EXECTOSTR '(' EXPR ')' { $$ = sm_new_expr(SM_EXECTOSTR_EXPR,(sm_object*)$3, _note());}
| OS_GETENV '(' EXPR ')' { $$ = sm_new_expr(SM_OS_GETENV_EXPR,(sm_object*)$3, _note());}
| OS_SETENV '(' EXPR ',' EXPR ')' { $$ = sm_new_expr_2(SM_OS_SETENV_EXPR,(sm_object*)$3,(sm_object*)$5, _note());}
| FORK '(' ')' { $$ = sm_new_expr_n(SM_FORK_EXPR,0,0, _note());}
| WAIT '(' ')' { $$ = sm_new_expr_n(SM_WAIT_EXPR,0,0, _note());}
| LS '(' ')' { $$ = sm_new_expr_n(SM_LS_EXPR,0,0, _note());}
| CD '(' EXPR ')' { $$ = sm_new_expr(SM_CD_EXPR,(sm_object*)$3, _note());}
| PWD '(' ')' { $$ = sm_new_expr_n(SM_PWD_EXPR,0,0, _note());}
| RUNTIME_META '(' EXPR ')' { $$ = sm_new_expr(SM_RUNTIME_META_EXPR, (sm_object*)$3, _note());}
| GC '(' ')' { $$ = sm_new_expr_n(SM_GC_EXPR, 0,0, _note()); }
| IS_ERR '(' EXPR ')' { $$ = sm_new_expr(SM_ISERR_EXPR,(sm_object*)$3,_note());}
| ERR_TITLE '(' EXPR ')' { $$ = sm_new_expr(SM_ERRTITLE_EXPR,(sm_object*)$3,_note());}
| ERR_MESSAGE '(' EXPR ')' { $$ = sm_new_expr(SM_ERRMESSAGE_EXPR,(sm_object*)$3,_note());}
| ERR_SOURCE '(' EXPR ')' { $$ = sm_new_expr(SM_ERRSOURCE_EXPR,(sm_object*)$3,_note());}
| ERR_LINE '(' EXPR ')' { $$ = sm_new_expr(SM_ERRLINE_EXPR,(sm_object*)$3,_note());}
| ERR_NOTES '(' EXPR ')' { $$ = sm_new_expr(SM_ERRNOTES_EXPR,(sm_object*)$3,_note());}


ERROR : '<' '>' { $$ = sm_new_error(0,NULL,0 ,NULL,parsing_fpath_len,parsing_fpath,yylineno);}
| '<' SYM '>' {
    sm_symbol* sym = (sm_symbol*)$2; // Assuming $3 is the EXPR result and it's a sm_string*
    $$ = sm_new_error(sym->name->size, &sym->name->content,0, "",parsing_fpath_len, parsing_fpath,yylineno);
}
| '<' SYM ',' EXPR '>' {
    sm_symbol* title = (sm_symbol*)$2; // Assuming $3 is the EXPR result and it's a sm_string*
    sm_string* message = (sm_string*)$4; // Assuming $3 is the EXPR result and it's a sm_string*
    $$ = sm_new_error(title->name->size, &title->name->content,message->size, &message->content,parsing_fpath_len, parsing_fpath,yylineno);
}

FUN : FUN_INTRO EXPR {
  //local variables are changed from symbol to local
  $1->content = (sm_object *)sm_localize((sm_object*)$2, $1);
  $$ = $1;
}

FUN_INTRO : PARAM_LIST ARROW {
  sm_cx * parent_cx = *(sm_global_lex_stack(NULL)->top);
  sm_fun *new_fun = sm_new_fun(parent_cx, ($1)->size, (sm_object *)NULL);
  for (uint32_t i = 0; i < ($1)->size; i++) {
    sm_fun_param_obj *po = (sm_fun_param_obj *)sm_expr_get_arg($1, i);
    sm_fun_set_param(new_fun, i, po->name, po->default_val);
  }
  $$ = new_fun;
}
| SYM ARROW {
  sm_cx * parent_cx = *(sm_global_lex_stack(NULL)->top);
  sm_fun *new_fun = sm_new_fun(parent_cx,1, (sm_object *)NULL);
  sm_fun_set_param(new_fun, 0, $1->name, NULL);
  $$ = new_fun;
}

F64_ARRAY : F64_ARRAY_LIST ']' {};
| F64_ARRAY_LIST ',' ']' {};
| F64_ARRAY_OPEN INTEGER ']' {
  sm_space * space = sm_new_space(sizeof(f64));
  $$ = sm_new_array(SM_F64_TYPE, 1,(sm_object*)space,sizeof(sm_space)) ;
  sm_f64_array_set($$,0,$2);
}
| F64_ARRAY_OPEN F64 ']' {
  sm_space * space = sm_new_space(sizeof(f64));
  $$ = sm_new_array(SM_F64_TYPE, 1,(sm_object*)space,sizeof(sm_space)) ;
  sm_f64_array_set($$,0,((sm_f64*)$2)->value);
} 
| F64_ARRAY_OPEN  ']' { $$ = sm_new_array(SM_F64_TYPE, 0,NULL,0) ;} 


F64_ARRAY_LIST : F64_ARRAY_OPEN F64 ',' F64 {
 $$ = sm_new_array(SM_F64_TYPE,2,NULL,0);
 sm_space* space= sm_new_space(sizeof(f64)*2);
 $$->content=(sm_object*)space;
 sm_f64_array_set($$,0,((sm_f64*)$2)->value);
 sm_f64_array_set($$,1,((sm_f64*)$4)->value);
}
| F64_ARRAY_LIST ',' INTEGER {
      $$->size++;
      sm_f64_array_set($$,$$->size-1,$3);
};
| F64_ARRAY_LIST ',' F64 {
      $$->size++;
      sm_f64_array_set($$,$$->size-1,((sm_f64*)$3)->value);
};

UI8_ARRAY : UI8_ARRAY_OPEN ']' {};
| UI8_ARRAY_LIST ',' ']' {};
| UI8_ARRAY_OPEN INTEGER ']' {
  sm_space * space = sm_new_space(1);
  $$ = sm_new_array(SM_UI8_TYPE, 1,(sm_object*)space,sizeof(sm_space)) ;
  sm_ui8_array_set($$,0,$2);
} 
| UI8_ARRAY_LIST ']' {}; 

UI8_ARRAY_LIST : UI8_ARRAY_OPEN INTEGER ',' INTEGER  {
     $$ = sm_new_array(SM_UI8_TYPE,2,NULL,0); 
     sm_space * space = sm_new_space(2);
     $$->content=(sm_object*)space;
     sm_ui8_array_set($$,0,$2);
     sm_ui8_array_set($$,1,$4);
}
| UI8_ARRAY_LIST ',' INTEGER {
      $$->size++;
      sm_ui8_array_set($$,$$->size-1,$3);
};

PARAM_LIST : '(' ')' {
  $$ = sm_new_expr_n(SM_PARAM_LIST_EXPR, 0, 0, _note());
}
| '(' SYM ')' {
  sm_fun_param_obj * fpo = sm_new_fun_param_obj($2->name,NULL);
  $$                = sm_new_expr(SM_PARAM_LIST_EXPR,(sm_object*)fpo, _note());
}
| PARAM_LIST_OPEN ')' {}
| PARAM_LIST_OPEN SYM ')' {
  sm_fun_param_obj * new_param= sm_new_fun_param_obj($2->name,NULL);
  $$ = sm_expr_append($1, (sm_object *)new_param);
}

PARAM_LIST_OPEN : '(' SYM ',' {
  sm_expr *new_expr = sm_new_expr_n(SM_PARAM_LIST_EXPR, 1, 3, _note());
  sm_fun_param_obj * new_param= sm_new_fun_param_obj($2->name,NULL);
  $$                = sm_expr_set_arg(new_expr, 0, (sm_object*)new_param);
}
| PARAM_LIST_OPEN SYM ',' {
  sm_fun_param_obj * new_param= sm_new_fun_param_obj($2->name,NULL);
  $$ = sm_expr_append($1, (sm_object *)new_param);
}

DOT_ASSIGNMENT : EXPR DOT SYM '=' EXPR { $$ = sm_new_expr_3(SM_ASSIGN_DOT_EXPR, (sm_object *)($1), (sm_object *)($3), (sm_object *)($5), _note()); }
| SYM DOT SYM '=' EXPR { $$ = sm_new_expr_3(SM_ASSIGN_DOT_EXPR, (sm_object *)($1), (sm_object *)($3), (sm_object *)($5), _note()); }
| EXPR '=' EXPR { $$ = sm_new_expr_2(SM_ASSIGN_DOT_EXPR, (sm_object *)($1), (sm_object *)($3), _note()); }

ASSIGNMENT : SYM '=' EXPR { $$ = sm_new_expr_2(SM_ASSIGN_EXPR, (sm_object *)($1), (sm_object *)($3), _note()); }

INDEX_ASSIGNMENT : EXPR '[' EXPR ']' '=' EXPR { $$ = sm_new_expr_3(SM_ASSIGN_INDEX_EXPR, (sm_object *)($1), (sm_object *)($3), (sm_object *)($6), _note()); }
| SYM '[' EXPR ']' '=' EXPR { $$ = sm_new_expr_3(SM_ASSIGN_INDEX_EXPR, (sm_object *)($1), (sm_object *)($3), (sm_object *)($6), _note()); }

BLOCK : OPEN_BLOCK '}' {
  sm_stack_pop(sm_global_lex_stack(NULL));
}
| OPEN_BLOCK ';' '}' {
  sm_stack_pop(sm_global_lex_stack(NULL));
}

OPEN_BLOCK : '{' EXPR {
  sm_cx * new_cx = sm_new_cx((sm_cx*)*sm_global_lex_stack(NULL)->top);
  sm_cx_contextualize((sm_object*)$2,new_cx);
  sm_global_lex_stack(sm_stack_push(sm_global_lex_stack(NULL), new_cx));
  $$ = sm_new_expr_2(SM_BLOCK_EXPR,(sm_object*)new_cx, (sm_object *)$2, _note()); 
}
| OPEN_BLOCK ';' EXPR {
  sm_cx_contextualize((sm_object*)$3,*sm_global_lex_stack(NULL)->top);
  $$ = sm_expr_append((sm_expr *)$1, (sm_object *)$3);
}

TEST_LT : '<' '(' EXPR ',' EXPR ')' {
  $$ = sm_new_expr_2(SM_LT_EXPR, (sm_object *)($3), (sm_object *)($5), _note());
}
| EXPR '<' EXPR { $$ = sm_new_expr_2(SM_LT_EXPR, (sm_object *)($1), (sm_object *)($3), _note()); }

TEST_GT : '>' '(' EXPR ',' EXPR ')' {
  $$ = sm_new_expr_2(SM_GT_EXPR, (sm_object *)($3), (sm_object *)($5), _note());
}
| EXPR '>' EXPR { $$ = sm_new_expr_2(SM_GT_EXPR, (sm_object *)($1), (sm_object *)($3), _note()); }

TEST_LT_EQ: LT_EQ '(' EXPR ',' EXPR ')' {
  $$ = sm_new_expr_2(SM_LT_EQ_EXPR, (sm_object *)($3), (sm_object *)($5), _note());
}
| EXPR LT_EQ EXPR { $$ = sm_new_expr_2(SM_LT_EQ_EXPR,(sm_object*)$1,(sm_object*)$3, _note());};

TEST_GT_EQ: GT_EQ '(' EXPR ',' EXPR ')' {
  $$ = sm_new_expr_2(SM_GT_EQ_EXPR, (sm_object *)($3), (sm_object *)($5), _note());
}
| EXPR GT_EQ EXPR { $$ = sm_new_expr_2(SM_GT_EQ_EXPR,(sm_object*)$1,(sm_object*)$3, _note());};

TEST_ISNAN: ISNAN '(' EXPR ')' {
  $$ = sm_new_expr(SM_ISNAN_EXPR, (sm_object *)($3), _note());
}

TEST_ISINF: ISINF '(' EXPR ')' {
  $$ = sm_new_expr(SM_ISINF_EXPR, (sm_object *)($3), _note());
}

EQ : EQEQ '(' EXPR ',' EXPR ')' {
  $$ = sm_new_expr_2(SM_EQEQ_EXPR, (sm_object *)($3), (sm_object *)($5), _note());
}
| EXPR EQEQ EXPR { $$ = sm_new_expr_2(SM_EQEQ_EXPR, (sm_object *)($1), (sm_object *)($3), _note()); }

IF_STATEMENT : IF '(' EXPR ')' EXPR  {
  $$ = sm_new_expr_2(SM_IF_EXPR, (sm_object *)($3), (sm_object *)($5), _note());
}
| IF '(' EXPR ')' EXPR ELSE EXPR {
  $$ = sm_new_expr_3(SM_IF_ELSE_EXPR, (sm_object *)($3), (sm_object *)($5), (sm_object *)($7), _note());
}

FUN_CALL : FUN_CALL_OPEN ')' {}
| EXPR '(' ')'{
  // store the function in the fun_call here
  // index: 0   1
  // value: expr args
  sm_expr * args= sm_new_expr_n(SM_PARAM_LIST_EXPR,0,0, _note());
  $$ = sm_new_expr_2(SM_FUN_CALL_EXPR, (sm_object *)$1, (sm_object *)args, _note());
}

FUN_CALL_OPEN : EXPR '(' EXPR {
  // store the function in the fun_call here
  // index: 0   1
  // value: expr args
  sm_expr * args= sm_new_expr_n(SM_PARAM_LIST_EXPR,1,2, _note());
  args=sm_expr_set_arg(args,0,(sm_object*)$3);
  $$ = sm_new_expr_2(SM_FUN_CALL_EXPR, (sm_object *)$1, (sm_object *)args, _note());
}
| FUN_CALL_OPEN ',' EXPR {
  sm_expr * args = (sm_expr*)sm_expr_get_arg($1,1);
  args = sm_expr_append(args, (sm_object *)$3);
  $$=sm_expr_set_arg($1,1,(sm_object*)args);
}

META_EXPR : ':' EXPR { $$ = sm_new_meta((sm_object *)$2, *(sm_global_lex_stack(NULL))->top); }

TUPLE : TUPLE_LIST ']' {};
| TUPLE_LIST ',' ']' {};
| '[' EXPR ']' { $$ = sm_new_expr(SM_TUPLE_EXPR, (sm_object *)$2, _note()); }
| '[' ']' { $$ = sm_new_expr_n(SM_TUPLE_EXPR, 0, 0, _note()); }

TUPLE_LIST : '[' EXPR ',' EXPR { $$ = sm_new_expr_2(SM_TUPLE_EXPR, (sm_object *)$2, (sm_object *)$4, _note()); }
| TUPLE_LIST ',' EXPR { $$ = sm_expr_append($1, (sm_object *)$3); }

ASSOCIATION : EXPR ASSOCIATE EXPR { $$ = sm_new_expr_2(SM_ASSOCIATE_EXPR,(sm_object*)$1,(sm_object*)$3, _note()); }

CONTEXT : CONTEXT_LIST '}' {
  sm_stack_pop(sm_global_lex_stack(NULL));
}
| CONTEXT_LIST ';' '}' {
  sm_stack_pop(sm_global_lex_stack(NULL));
}
| '{' ASSOCIATION ';' '}' {
  sm_cx *parent_cx         = *(sm_global_lex_stack(NULL)->top);
  sm_cx *new_cx            = sm_new_cx(parent_cx);
  sm_object  *value        = (sm_object *)sm_expr_get_arg($2, 1);
  sm_cx_let(new_cx, (sm_symbol*)sm_expr_get_arg($2, 0), value);
  $$ = new_cx;
}
| '{' ASSOCIATION '}' {
  sm_cx *parent_cx         = *(sm_global_lex_stack(NULL)->top);
  sm_cx *new_cx            = sm_new_cx(parent_cx);
  sm_object  *value        = ((sm_object *)sm_expr_get_arg($2, 1));
  sm_cx_let(new_cx,(sm_symbol *)sm_expr_get_arg($2, 0),value);
  $$ = new_cx;
}
| '{' '}' {
  sm_cx *parent_cx = *(sm_global_lex_stack(NULL)->top);
  $$    = sm_new_cx(parent_cx);
}

CONTEXT_LIST : '{' ASSOCIATION ';' ASSOCIATION {
  sm_cx *parent_cx = *(sm_global_lex_stack(NULL)->top);
  sm_cx *new_cx    = sm_new_cx(parent_cx);
  sm_object        *value  = (sm_object *)sm_expr_get_arg($2, 1);
  sm_object        *value2 = (sm_object *)sm_expr_get_arg($4, 1);
  sm_cx_contextualize(value,new_cx);
  sm_cx_contextualize(value2,new_cx);
  sm_cx_let(new_cx,(sm_symbol*)sm_expr_get_arg($2,0),value);
  sm_cx_let(new_cx,(sm_symbol*)sm_expr_get_arg($4,0),value2);
  sm_global_lex_stack(sm_stack_push(sm_global_lex_stack(NULL), new_cx));
  $$ = new_cx;
}
| CONTEXT_LIST ';' ASSOCIATION {
  sm_string  *name   = ((sm_symbol *)sm_expr_get_arg($3, 0))->name;
  sm_object  *value  = (sm_object *)sm_expr_get_arg($3, 1);
  sm_cx_let($1,(sm_symbol*)sm_expr_get_arg($3,0),value);
  $$                 = $1;
}


%%


FILE* lex_file(char *fpath){
  if(access(fpath,F_OK)!=0){
    fprintf(stderr,"File not found: %s\n",fpath);
    return false;
  }
  FILE * f = _lex_file(fpath);
  strcpy(parsing_fpath,fpath);
  parsing_fpath_len = strlen(fpath);
  return f;
}

void done_lexing_file(FILE* f){
  parsing_fpath[0]='\0';
  parsing_fpath_len =0;
  _done_lexing_file(f);
}

void lex_cstr(char * cstr,int len){
  _lex_cstr(cstr,len);
}

void yyerror(const char *msg) {
  // Use this function to investigate the error.
  fprintf(stderr, "Parsing error on line: %i %s\n",yylineno, msg);
  //Parsing a file will stop here, but repl will continue and needs a line increment
  yylineno++;
}

// Return a context with source and line info
// For the parser to quickly create a common type of context
struct sm_cx *_note() {
  struct sm_cx * note = sm_new_cx(NULL);
  if(parsing_fpath[0])
    sm_cx_let(note,sm_new_symbol("source",6),(sm_object*)sm_new_string(parsing_fpath_len,parsing_fpath));
  else
    sm_cx_let(note,sm_new_symbol("source",6),(sm_object*)sm_new_string(7,"(stdin)"));
  sm_cx_let(note,sm_new_symbol("line",4),(sm_object*)sm_new_f64(yylineno));
  return note;
}
