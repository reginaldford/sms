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

[ \t\f\v\n]  { ; }
exit         {return EXIT;}
help         {return HELP;}

sin          {return SIN;}
cos          {return COS;}
tan          {return TAN;}
asin         {return ASIN;}
acos         {return ACOS;}
atan         {return ATAN;}
sinh         {return SINH;}
cosh         {return COSH;}
tanh         {return TANH;}
asin         {return ASINH;}
acos         {return ACOSH;}
atan         {return ATANH;}
sec          {return SEC;}
csc          {return CSC;}
cot          {return COT;}
sec          {return ASEC;}
csc          {return ACSC;}
cot          {return ACOT;}
sec          {return SECH;}
csc          {return CSCH;}
cot          {return COTH;}
sech         {return ASECH;}
csch         {return ACSCH;}
coth         {return ACOTH;}
exp          {return EXP;}
ln           {return LN;}
log          {return LOG;}
sqrt         {return SQRT;}
abs          {return ABS;}

diff         {return DIFF;}
simp         {return SIMP;}
int          {return INT;}

if           {return IF;}
while        {return WHILE;}
doWhile      {return DO_WHILE;}
eval         {return EVAL;}
evalInCX     {return EVAL_IN_CX;}
evalFast     {return EVAL_FAST;}
evalFastInCX {return EVAL_FAST_IN_CX;}
fails        {return FAILS;}
parse        {return PARSE;}
"toStr"      {return TO_STR;}
"toStrFmt"   {return TO_STRFMT;}
==           {return EQEQ;}
!=           {return NOTEQ;}
"<="         {return LT_EQ;}
">="         {return GT_EQ;}

not          {return NOT;}
or           {return OR;}
xor          {return XOR;}
and          {return AND;}

input        {return INPUT;}
put          {return PUT;}
putln        {return PUTLN;}

let          {return LET;}
self         {return SELF;}
clear        {return CLEAR;}
"."          {return DOT;}
parent       {return PARENT;}
rm           {return RM;}

newCx        {return NEW_CX;}
cxLet        {return CX_LET;}
cxContaining {return CX_CONTAINING;}
cxSet        {return CX_SET;}
cxRm         {return CX_RM;}
cxClear      {return CX_CLEAR;}
cxkeys       {return CX_KEYS;}
cxValues     {return CX_VALUES;}
cxSize       {return CX_SIZE;}
cxImport     {return CX_IMPORT;}
cxMap        {return CX_MAP;}

fileParse    {return FILE_PARSE;}
fileRun      {return FILE_RUN;}
fileRead     {return FILE_READ;}
filePart     {return FILE_PART;}
fileToblk    {return FILE_TOBLK;}
fileExists   {return FILE_EXISTS;}
fileStat     {return FILE_STAT;}
fileCp       {return FILE_CP;}
fileMv       {return FILE_MV;}
fileRm       {return FILE_RM;}
fileSize     {return FILE_SIZE;}
fileWrite    {return FILE_WRITE;}
fileAppend   {return FILE_APPEND;}

csvToArr   {return CSV_TOARR;}
arrToCsv   {return ARR_TOCSV;}
blkToFile  {return BLK_TOFILE;}

newArr      {return NEW_ARR;}
arrCat      {return ARR_CAT;}
arrRpt      {return ARR_REPEAT;}
part        {return PART;}
size        {return SIZE;}
map         {return MAP;}

random      {return RANDOM;}
seed        {return SEED;}
round       {return ROUND;}
floor       {return FLOOR;}
ceil        {return CEIL;}

=>          {return ARROW;}
newFn       {return NEW_FN;}
fnXpr       {return FN_XPR;}
fnSetxpr    {return FN_XPR;}
fnParams    {return FN_PARAMS;}
fnSetparams {return FN_PARAMS;}
fnParent    {return FN_PARENT;}
fnSetparent {return FN_SETPARENT;}

newXpr     {return NEW_XPR;}
xprOp      {return XPR_OP;}
xprSetop   {return XPR_SETOP;}
xprOpstr   {return XPR_OPSTR;}

newBlk     {return NEW_BLK;}
blkSet     {return BLK_SET;}
blkSize    {return BLK_SIZE;}
blkGet     {return BLK_GET;}
blkUnite   {return BLK_UNITE;}
blkPart    {return BLK_PART;}
blkPrint   {return BLK_PRINT;}
blkTonums  {return BLK_TONUMS;}
blkMap     {return BLK_MAP;}
blkTostr   {return BLK_TOSTR;}

memAlloc   {return MALLOC_OBJ;}
memFree    {return FREE_OBJ;}

cpy         {return COPY;}
deepCpy     {return DEEP_COPY;}

symName     {return SYM_NAME;}

newStr      {return NEW_STR;}
"str\+"     {return STR_CAT;}
"strRpt"    {return STR_REPEAT;}
strEscape   {return STR_ESCAPE;}
strUnescape {return STR_UNESCAPE;}
strSize     {return STR_SIZE;}
strFind     {return STR_FIND;}
strFindr    {return STR_FINDR;}
strGet      {return STR_GET;}
strSet      {return STR_SET;}
strSplit    {return STR_SPLIT;}
strPart     {return STR_PART;}
strCmp      {return STR_CMP;}
strToNums   {return STR_TONUMS;}
strMap      {return STR_MAP;}
strToBlk    {return STR_TOBLK;}

newNums    {return NEW_NUMS;}
numsToBlk  {return NUMS_TOBLK;}
numsToStr  {return NUMS_TOSTR;}
numsToCsv  {return NUMS_TOSTR;}

meta        {return RUNTIME_META;}

dateStr    {return DATE_STR;}
date       {return DATE;}
time       {return TIME;}
sleep      {return SLEEP;}

execFile      {return EXEC;}
forkFile      {return FORK;}

pwd          	 {return PWD;}
ls           	 {return LS;}
cd           	 {return CD;}
_thisProcess   {return THISPROCESS;}
_args          {return ARGS;}
_getOptions    {return GETOPTIONS;}
_setOptions    {return SETOPTIONS;}
_resetOptions  {return RESETOPTIONS;}
_gc            {return GC;}
_mem_saveFile  {return MEM_SAVEFILE;}
_mem_readFile  {return MEM_READFILE;}
_mem_checkFile {return MEM_CHECKFILE;}

<<EOF>>     {return DONE;}
'\0'        {return DONE;}

\.?{digit}+(\.{digit}+)?([eE]-?{digit}+)?  {
   yylval.num = sm_new_double(atof(yytext));
   return NUM;
  }
[\<\>]\= { return yytext[0]; }

[-+*/=\^\;\[\]\(\)\{\},\:\<\>] { return yytext[0];}

({alpha}?+{digit}?+_?+)?+\'?+ {
    yylval.sym = sm_new_symbol(sm_new_string(yyleng,yytext));
    return SYM;
  }

"#".* {/*comment*/};


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
