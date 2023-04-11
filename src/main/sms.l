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
help        {return HELP;}

sin         {return SIN;}
cos         {return COS;}
tan         {return TAN;}
asin        {return ASIN;}
acos        {return ACOS;}
atan        {return ATAN;}
sinh        {return SINH;}
cosh        {return COSH;}
tanh        {return TANH;}
asin        {return ASINH;}
acos        {return ACOSH;}
atan        {return ATANH;}
sec         {return SEC;}
csc         {return CSC;}
cot         {return COT;}
sec         {return ASEC;}
csc         {return ACSC;}
cot         {return ACOT;}
sec         {return SECH;}
csc         {return CSCH;}
cot         {return COTH;}
sech        {return ASECH;}
csch        {return ACSCH;}
coth        {return ACOTH;}
exp         {return EXP;}
ln          {return LN;}
log         {return LOG;}
sqrt        {return SQRT;}
abs         {return ABS;}

diff        {return DIFF;}
simp        {return SIMP;}
int         {return INT;}

if          {return IF;}
while       {return WHILE;}
dowhile     {return DO_WHILE;}
eval        {return EVAL;}
eval_fast   {return EVAL_FAST;}
parse       {return PARSE;}
"to_str"    {return TO_STR;}
"to_strfmt" {return TO_STRFMT;}
==          {return EQEQ;}
!=          {return NOTEQ;}
"<="        {return LT_EQ;}
">="        {return GT_EQ;}

not         {return NOT;}
or          {return OR;}
xor         {return XOR;}
and         {return AND;}

input       {return INPUT;}
print      {return PRINT;}

let         {return LET;}
self        {return SELF;}
clear       {return CLEAR;}
"."         {return DOT;}
parent      {return PARENT;}
rm          {return RM;}

new_cx        {return NEW_CX;}
cx_let        {return CX_LET;}
cx_containing {return CX_CONTAINING;}
cx_set        {return CX_SET;}
cx_rm         {return CX_RM;}
cx_clear      {return CX_CLEAR;}
cx_keys       {return CX_KEYS;}
cx_values     {return CX_VALUES;}
cx_size       {return CX_SIZE;}
cx_import     {return CX_IMPORT;}
cx_map        {return CX_MAP;}

file_parse  {return FILE_PARSE;}
file_tostr  {return FILE_TOSTR;}
file_toblk  {return FILE_TOBLK;}
str_tofile  {return STR_TOFILE;}
csv_toarr   {return CSV_TOARR;}
arr_tocsv   {return ARR_TOCSV;}
blk_tofile  {return BLK_TOFILE;}

new_arr     {return NEW_ARR;}
arr+        {return ARR_CAT;}
arr*        {return ARR_REPEAT;}
part        {return PART;}
size        {return SIZE;}
map         {return MAP;}

random      {return RANDOM;}
seed        {return SEED;}
round       {return ROUND;}
floor       {return FLOOR;}
ceil        {return CEIL;}

=>           {return ARROW;}
new_fn       {return NEW_FN;}
fn_xpr       {return FN_XPR;}
fn_setxpr    {return FN_XPR;}
fn_params    {return FN_PARAMS;}
fn_setparams {return FN_PARAMS;}
fn_parent    {return FN_PARENT;}
fn_setparent {return FN_SETPARENT;}

new_xpr     {return NEW_XPR;}
xpr_op      {return XPR_OP;}
xpr_setop   {return XPR_SETOP;}
xpr_opstr   {return XPR_OPSTR;}

new_blk     {return NEW_BLK;}
blk_set     {return BLK_SET;}
blk_size    {return BLK_SIZE;}
blk_get     {return BLK_GET;}
blk_unite   {return BLK_UNITE;}
blk_part    {return BLK_PART;}
blk_print   {return BLK_PRINT;}
blk_tonums  {return BLK_TONUMS;}
blk_map     {return BLK_MAP;}
blk_tostr   {return BLK_TOSTR;}

mem_alloc   {return MALLOC_OBJ;}
mem_free    {return FREE_OBJ;}

cpy         {return COPY;}
deep_cpy    {return DEEP_COPY;}

sym_name     {return SYM_NAME;}

new_str      {return NEW_STR;}
"str+"       {return STR_CAT;}
"str*"       {return STR_REPEAT;}
str_escape   {return STR_ESCAPE;}
str_unescape {return STR_UNESCAPE;}
str_size     {return STR_SIZE;}
str_find     {return STR_FIND;}
str_findr    {return STR_FINDR;}
str_getchar  {return STR_GET;}
str_setchar  {return STR_SET;}
str_split    {return STR_SPLIT;}
str_part     {return STR_PART;}
str_cmp      {return STR_CMP;}
str_tonums   {return STR_TONUMS;}
str_map      {return STR_MAP;}
str_toblk    {return STR_TOBLK;}

new_nums    {return NEW_NUMS;}
nums_toblk  {return NUMS_TOBLK;}
nums_tostr  {return NUMS_TOSTR;}
nums_tocsv  {return NUMS_TOSTR;}

date_str    {return DATE_STR;}
date        {return DATE;}
time        {return TIME;}
sleep       {return SLEEP;}

exec_file        {return EXEC;}
fork_file        {return FORK;}

pwd           {return PWD;}
ls            {return LS;}
cd            {return CD;}
_thisprocess   {return THISPROCESS;}
_getargs       {return GETARGS;}
_getoptions    {return GETOPTIONS;}
_setoptions    {return SETOPTIONS;}
_resetoptions  {return RESETOPTIONS;}
_gc            {return GC;}
_mem_savefile  {return MEM_SAVEFILE;}
_mem_readfile  {return MEM_READFILE;}
_mem_checkfile {return MEM_CHECKFILE;}

<<EOF>>     {return DONE;}
'\0'        {return DONE;}

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
  YY_BUFFER_STATE bs = yy_create_buffer(f,YY_BUF_SIZE);
  yypush_buffer_state(bs);
}

void _done_lexing_file(){
  yypop_buffer_state();
}

void _lex_cstr(char * cstr,int len){
  yy_scan_bytes(cstr,len);
}
