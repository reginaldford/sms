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

[ \t\f\v\n\\]  { ; }
_version     {return VERSION;}
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
sec          {return SEC;}
csc          {return CSC;}
cot          {return COT;}
asec         {return ASEC;}
acsc         {return ACSC;}
acot         {return ACOT;}
asech        {return ASECH;}
acsch        {return ACSCH;}
acoth        {return ACOTH;}
exp          {return EXP;}
ln           {return LN;}
log          {return LOG;}
sqrt         {return SQRT;}
abs          {return ABS;}

"++"         {return INC;}
"--"         {return DEC;}
diff         {return DIFF;}
simp         {return SIMP;}
int          {return INT;}

if           {return IF;}
else         {return ELSE;}
while        {return WHILE;}
for          {return FOR;}
doWhile      {return DO_WHILE;}
return       {return RETURN;}
eval         {return EVAL;}
cxEval       {return CX_EVAL;}
evalFast     {return EVAL_FAST;}
evalFastInCX {return EVAL_FAST_IN_CX;}
fails        {return FAILS;}
parse        {return PARSE;}
str          {return NEW_STR;}
toStrFmt     {return TO_STRFMT;}

"->"         {return ASSOCIATE;}
==           {return EQEQ;}
!=           {return NOTEQ;}
"<="         {return LT_EQ;}
">="         {return GT_EQ;}

isNan        {return ISNAN;};
isInf        {return ISINF;};


is           {return IS;}

not          {return NOT;}
or           {return OR;}
xor          {return XOR;}
and          {return AND;}

input        {return INPUT;}
put          {return PUT;}
putLn        {return PUTLN;}

let          {return LET;}
self         {return SELF;}
args         {return ARGS;}
clear        {return CLEAR;}
"."          {return DOT;}
parent       {return PARENT;}
rm           {return RM;}

"f64["       {return F64_ARRAY_OPEN;}
"ui8["        {return UI8_ARRAY_OPEN;}

newCx        {return NEW_CX;}
cxSetParent  {return CX_SETPARENT;}
cxLet        {return CX_LET;}
cxGet        {return CX_GET;}
cxHas        {return CX_HAS;}
cxGetFar     {return CX_GET_FAR;}
cxHasFar     {return CX_HAS_FAR;}
cxContaining {return CX_CONTAINING;}
cxSet        {return CX_SET;}
cxRm         {return CX_RM;}
cxClear      {return CX_CLEAR;}
cxKeys       {return CX_KEYS;}
cxValues     {return CX_VALUES;}
cxSize       {return CX_SIZE;}
cxImport     {return CX_IMPORT;}
cxMap        {return CX_MAP;}
cxReduce     {return CX_REDUCE;}

fileParse    {return FILE_PARSE;}
fileRun      {return FILE_RUN;}
fileRead     {return FILE_READ;}
filePart     {return FILE_PART;}
fileTomem    {return FILE_TOMEM;}
fileExists   {return FILE_EXISTS;}
fileStat     {return FILE_STAT;}
fileCp       {return FILE_CP;}
fileMv       {return FILE_MV;}
fileRm       {return FILE_RM;}
fileSize     {return FILE_SIZE;}
fileWrite    {return FILE_WRITE;}
fileWriteTga {return FILE_WRITETGA;}
fileAppend   {return FILE_APPEND;}

memToFile  {return MEM_TOFILE;}

zeros       {return ZEROS;}
cat         {return CAT;}
repeat      {return REPEAT;}
part        {return PART;}
size        {return SIZE;}
map         {return MAP;}
reduce      {return REDUCE;}

random      {return RANDOM;}
seed        {return SEED;}
round       {return ROUND;}
floor       {return FLOOR;}
ceil        {return CEIL;}
mod         {return MOD;}

=>          {return ARROW;}
newFn       {return NEW_FN;}
fnXp        {return FN_XP;}
fnSetXp     {return FN_SETXP;}
fnParams    {return FN_PARAMS;}
fnSetParams {return FN_SETPARAMS;}
fnParent    {return FN_PARENT;}
fnSetParent {return FN_SETPARENT;}

newXp       {return NEW_XP;}
xpOp        {return XP_OP;}
xpSetOp     {return XP_SETOP;}
xpOpSym     {return XP_OPSYM;}

newMem     {return NEW_MEM;}
memSet     {return MEM_SET;}
memSize    {return MEM_SIZE;}
memGet     {return MEM_GET;}
memUnite   {return MEM_UNITE;}
memPart    {return MEM_PART;}
memPrint   {return MEM_PRINT;}
memTonums  {return MEM_TONUMS;}
memMap     {return MEM_MAP;}
memTostr   {return MEM_TOSTR;}

memAlloc   {return MALLOC_OBJ;}
memFree    {return FREE_OBJ;}

cpy         {return COPY;}
deepCpy     {return DEEP_COPY;}

symName     {return SYM_NAME;}

newStr      {return NEW_STR;}
"str\+"     {return STR_CAT;}
strRepeat      {return STR_REPEAT;}
strEscape   {return STR_ESCAPE;}
strUnescape {return STR_UNESCAPE;}
strSize     {return STR_SIZE;}
strMut      {return STR_MUT;}
strFind     {return STR_FIND;}
strFindR    {return STR_FINDR;}
strGet      {return STR_GET;}
strSet      {return STR_SET;}
strSplit    {return STR_SPLIT;}
strPart     {return STR_PART;}
strCmp      {return STR_CMP;}
strToNums   {return STR_TONUMS;}
strMap      {return STR_MAP;}
strToMem    {return STR_TOMEM;}

newNums    {return NEW_NUMS;}
numsToMem  {return NUMS_TOMEM;}
numsToStr  {return NUMS_TOSTR;}
numsToCsv  {return NUMS_TOSTR;}

meta       {return RUNTIME_META;}

dateStr    {return DATE_STR;}
date       {return DATE;}
time       {return TIME;}
sleep      {return SLEEP;}

osFork        {return FORK;}
osWait        {return WAIT;}
osExec        {return OS_EXEC;}
osExecToStr   {return OS_EXECTOSTR;}
osGetEnv      {return OS_GETENV;}
osSetEnv      {return OS_SETENV;}

pwd          	 {return PWD;}
ls           	 {return LS;}
cd           	 {return CD;}

isErr         {return IS_ERR;}
errTitle      {return ERR_TITLE;}
errMessage    {return ERR_MESSAGE;}
errSource     {return ERR_SOURCE;}
errLine       {return ERR_LINE;}
errNotes      {return ERR_NOTES;}

_thisProcess   {return THISPROCESS;}
_getOptions    {return GETOPTIONS;}
_setOptions    {return SETOPTIONS;}
_gc            {return GC;}
_mem_saveFile  {return MEM_SAVEFILE;}
_mem_readFile  {return MEM_READFILE;}
_mem_checkFile {return MEM_CHECKFILE;}

<<EOF>>     {return ENDOFFILE;}
'\0'        {return DONE;}

\.?{digit}+(\.{digit}+)?([eE]-?{digit}+)?  {
   yylval.num = sm_new_f64(atof(yytext));
   return F64;
  }


ui8\(([0-9]+)\)    {
      int number = atoi(yytext + 4); // Skip "ui8("
      // Check for closing parenthesis
      if (yytext[4 + strlen(yytext + 4) - 1] != ')') {
          fprintf(stderr, "Syntax error: Missing closing parenthesis\n");
          return 0; // or appropriate error handling
      }
      yylval.ui8 = sm_new_ui8(number); // Call the function with the number
      return UI8;
  }


[\<\>\=] { return yytext[0]; }

[-+*/=\^\;\[\]\(\)\{\},\:\<\>] { return yytext[0];}


({alpha}?+{digit}?+_?+\'?+)?+ {
    yylval.sym = sm_new_symbol(yytext,yyleng);
    return SYM;
  }

"#".* {/*comment*/};


\"([^"\\]|\\.)*\" {
  // Text after the first quotation will be at &(yytext[1])
  // We also have string length yyleng-2, for trimming quotes.
  char *text = &(yytext[1]);

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
        case 'x': {
          // Hex escape code
          char hex[3] = {text[i + 2], text[i + 3], '\0'};
          int value = (int)strtol(hex, NULL, 16);
          text[final_i] = (char)value;
          i += 2;
          break;
        }
        default: // Unrecognized escape. backslash as default
          text[final_i] = '\\';
          break;
      }
      i += 2; // Skip the escape code
    } else {
      // Pass through regular characters
      text[final_i] = text[i];
      i++;
    }
    final_i++;
  }
  yylval.str = sm_new_string(final_i, text);
  return STRING;
}



%%


FILE* _lex_file(char * fpath){
  FILE * f = fopen(fpath,"rb");
  if(!f) return NULL;
  YY_BUFFER_STATE bs = yy_create_buffer(f,YY_BUF_SIZE);
  yypush_buffer_state(bs);
  return f;
}

void _done_lexing_file(FILE* f){
  yypop_buffer_state();
  fclose(f);
}

void _lex_cstr(char * cstr,int len){
  yy_scan_bytes(cstr,len);
}
