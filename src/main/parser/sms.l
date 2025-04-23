%{
// The following file is provided under the BSD 2-clause license. For more info, read LICENSE.txt.

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

f64  { return NEW_F64;  }
i64  { return NEW_I64;  }
i64\(\ +(-?{digit}+)\ +\) {
    char *num_str = yytext + 4;  // Skip "i64("
    // Find the position of the closing parenthesis
    char *end_paren = strchr(num_str, ')');
    if (!end_paren) {
        fprintf(stderr, "Syntax error: Missing closing parenthesis\n");
        return 0;
    }   
    // Temporarily terminate the string at the closing parenthesis
    *end_paren = '\0';
    // Trim spaces from the start and end of num_str
    char *end = num_str + strlen(num_str) - 1;
    end[1] = '\0';  // Null-terminate the trimmed string
    int64_t number = strtoll(num_str,NULL,10);  // Convert the trimmed number string to an integer
    yylval.i64 = number;
    return I64;
}
ui64 { return NEW_UI64; }
ui64\(\ +(-?{digit}+)\ +\) {
    char *num_str = yytext + 5;  // Skip "ui64("
    // Find the position of the closing parenthesis
    char *end_paren = strchr(num_str, ')');
    if (!end_paren) {
        fprintf(stderr, "Syntax error: Missing closing parenthesis\n");
        return 0;
    }   
    // Temporarily terminate the string at the closing parenthesis
    *end_paren = '\0';
    // Trim spaces from the start and end of num_str
    char *end = num_str + strlen(num_str) - 1;
    end[1] = '\0';  // Null-terminate the trimmed string
    uint64_t number = strtoull(num_str,NULL,10);  // Convert the trimmed number string to an integer
    yylval.ui64 = number;
    return UI64;
}
ui8  { return NEW_UI8;  }
ui8\(\ +(-?{digit}+)\ +\) {
    char *num_str = yytext + 4;  // Skip "ui8("
    // Find the position of the closing parenthesis
    char *end_paren = strchr(num_str, ')');
    if (!end_paren) {
        fprintf(stderr, "Syntax error: Missing closing parenthesis\n");
        return 0;
    }   
    // Temporarily terminate the string at the closing parenthesis
    *end_paren = '\0';
    // Trim spaces from the start and end of num_str
    char *end = num_str + strlen(num_str) - 1;
    end[1] = '\0';  // Null-terminate the trimmed string
    int number = atoi(num_str);  // Convert the trimmed number string to an integer
    yylval.ui8 = number;
    return UI8;
}


f64Repeat  { return F64_REPEAT;  }
i64Repeat  { return I64_REPEAT;  }
ui64Repeat { return UI64_REPEAT; }
ui8Repeat  { return UI8_REPEAT ; }

"&" {return IAND;}
"|" {return IOR;}
"^" {return IXOR;}

"&=" {return IAND_EQ;}
"|=" {return IOR_EQ;}
"^=" {return IXOR_EQ;}
"_~=" {return INOT_EQ;}

"!+"  {return IPLUS;}
"!-"  {return IMINUS;}
"!*"  {return ITIMES;}
"!/"  {return IDIVIDE;}
"!**" {return IPOW;}

"!+="   {return IPLUSEQ;}
"!-="   {return IMINUSEQ;}
"!*="   {return ITIMESEQ;}
"!/="   {return IDIVIDEEQ;}
"!**="  {return IPOWEREQ;}

"+"  {return PLUS;}
"-"  {return MINUS;}
"*"  {return TIMES;}
"/"  {return DIVIDE;}
"**" {return POW;}

"+="  {return PLUSEQ;}
"-="  {return MINUSEQ;}
"*="  {return TIMESEQ;}
"/="  {return DIVIDEEQ;}
"**=" {return POWEREQ;}

sin          {return SIN;}
cos          {return COS;}
tan          {return TAN;}
asin         {return ASIN;}
acos         {return ACOS;}
atan         {return ATAN;}
sinh         {return SINH;}
cosh         {return COSH;}
tanh         {return TANH;}
asinh        {return ASINH;}
acosh        {return ACOSH;}
atanh        {return ATANH;}

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
"!++"        {return IINC;}
"!--"        {return IDEC;}

diff         {return DIFF;}
simp         {return SIMP;}
int          {return INT;}

if           {return IF;}
else         {return ELSE;}
while        {return WHILE;}
for          {return FOR;}
in           {return IN;}
where        {return WHERE;}
doWhile      {return DO_WHILE;}
return       {return RETURN;}
eval         {return EVAL;}
cxEval       {return CX_EVAL;}
parse        {return PARSE;}
str          {return NEW_STR;}
toStrFmt     {return TO_STRFMT;}

":"         {return ASSOCIATE;}
==           {return EQEQ;}
"<="         {return LT_EQ;}
">="         {return GT_EQ;}

isNan        {return ISNAN;};
isInf        {return ISINF;};

is           {return IS;}

"~"          {return NOT;}
"||"         {return OR;}
"^^"         {return XOR;}
"&& "        {return AND;}

"~="         {return NOT_EQ;}
"||="        {return OR_EQ;}
"^^="        {return XOR_EQ;}
"&&="        {return AND_EQ;}

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
"i64["       {return I64_ARRAY_OPEN;}
"ui64["       {return UI64_ARRAY_OPEN;}
"ui8["       {return UI8_ARRAY_OPEN;}

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

fileParse      {return FILE_PARSE;}
fileRun        {return FILE_RUN;}
fileReadStr    {return FILE_READSTR;}
fileRead       {return FILE_READ;}
filePart       {return FILE_PART;}
fileTomem      {return FILE_TOMEM;}
fileExists     {return FILE_EXISTS;}
fileStat       {return FILE_STAT;}
fileCp         {return FILE_CP;}
fileMv         {return FILE_MV;}
fileRm         {return FILE_RM;}
fileSize       {return FILE_SIZE;}
fileWriteStr   {return FILE_WRITESTR;}
fileWriteArr   {return FILE_WRITEARR;}
fileAppend     {return FILE_APPEND;}

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

"=>"        {return ARROW;}
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

cpy         {return COPY;}
deepCpy     {return DEEP_COPY;}

symName     {return SYM_NAME;}

newStr      {return NEW_STR;}
"str\+"     {return STR_CAT;}
strRepeat   {return STR_REPEAT;}
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
strMap      {return STR_MAP;}

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

import        { return IMPORT; }

_thisProcess   {return THISPROCESS;}
_gc            {return GC;}
soLoad         {return SO_LOAD;}
soFun          {return SO_FUN;}
soUnload       {return SO_UNLOAD;}
ffSig          {return FF_SIG;}
ff             {return FF;}
_heapSave      {return HEAPSAVE;}
_imgSave       {return HEAPSAVE;}

<<EOF>>     {return ENDOFFILE;}
'\0'        {return DONE;}

({digit}+) {
 yylval.integer = strtoull(yytext,NULL,10);
 return INTEGER;
}

\.?{digit}+(\.{digit}+)([eE]-?{digit}+)?  {
   yylval.num = atof(yytext);
   return F64;
  }

[\<\>\=] { return yytext[0]; }

[-+*/=\^\;\[\]\(\)\{\},\@\<\>] { return yytext[0];}

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
