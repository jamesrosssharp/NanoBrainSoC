%{
/*=============================================================================
                                                    _
         .-==-.                   ..--/\/\--..     (_)
    __      ___    __      ____  ___  ____  ___    ____
   /  \  /\/ _ \/\/  \  /\/ _  \/ _ \/ _  \/ _ \/\/ /  \  /\
  / /\ \/ / (_)  / /\ \/ / (_) / (_)/ (_) / (_)  / / /\ \/ /
  \/  \__/\__/\_/\/  \__/\____/ (_) \ __ (\__/\_/_/\/  \__/
                              \_____//  \_\
   .-==-.           --===/\/\/\===---             ..--/\/\--..

    NanoBrain C Compiler (c) 2017 James Ross Sharp

=============================================================================*/

#include "gen-nbcc-parser.hpp"

using namespace std;
#define YY_DECL extern "C" int yylex()
int line_num = 1;


#define MAX_STRING 255
char g_string[MAX_STRING];
int g_str_idx = 0;

%}

%x COMMENTS
%x COMMENT
%x STRING_LITERAL
%x CHAR_LITERAL

%%
\/\*                    { BEGIN(COMMENTS); }
<COMMENTS>\*\/          { BEGIN(INITIAL);  }
<COMMENTS>\n      { ++line_num; }
<COMMENTS>.    ;

[ \t]   { /* strip whitespace */ }  ;

\/\/    { BEGIN(COMMENT); }
<COMMENT>\n { ++line_num; BEGIN(INITIAL); }
<COMMENT>. ;

\"  { g_str_idx = 0; BEGIN(STRING_LITERAL); }
<STRING_LITERAL>\n { ++line_num; }
<STRING_LITERAL>\" { BEGIN(INITIAL); g_string[g_str_idx] = '\0'; yylval.stringLiteral = strdup(g_string); return STR_LITERAL; }
<STRING_LITERAL>. { if (g_str_idx < MAX_STRING - 2) g_string[g_str_idx++] = yytext[0];};


\'  { g_str_idx = 0; BEGIN(CHAR_LITERAL); }
<CHAR_LITERAL>\n { ++line_num; }
<CHAR_LITERAL>\' { BEGIN(INITIAL); g_string[g_str_idx] = '\0'; yylval.stringLiteral = strdup(g_string); return CH_LITERAL; }
<CHAR_LITERAL>. { if (g_str_idx < MAX_STRING - 2) g_string[g_str_idx++] = yytext[0];};


"void"            { return TYPE_VOID; }
"unsigned int"    { return TYPE_UNSIGNED_INT; }
"unsigned short"  { return TYPE_UNSIGNED_SHORT; }
"unsigned char"   { return TYPE_UNSIGNED_CHAR; }
"unsigned long long int" {return TYPE_UNSIGNED_LONG; }
"unsigned long long" {return TYPE_UNSIGNED_LONG; }
"unsigned long"   { return TYPE_UNSIGNED_LONG; }

"int"             { return TYPE_INT;   /*  32 bits */ }
"short"           { return TYPE_SHORT; /*  16 bits */ }
"char"            { return TYPE_CHAR;  /*  8 bits  */ }
"long long"       { return TYPE_LONG;  /* 64 bits */  }
"long long int"   { return TYPE_LONG;  /* 64 bits */  }
"long"            { return TYPE_LONG;  /*  64 bits */ }


"asm"              { return ASM; }

"volatile"         { return VOLATILE; }
"const"            { return CONST;    }
"static"           { return STATIC;   }
"auto"             { return AUTO;     }

"return"          { return RETURN; }
"while"           { return WHILE;  }
"if"              { return IF;     }
"switch"          { return SWITCH; }

[0-9]+ {yylval.integerLiteral = atoi(yytext); return INT; }
0x[0-9a-f]+ {yylval.integerLiteral = strtol(yytext, nullptr, 16); return INT;}
[a-zA-Z_0-9]+ { yylval.symbol = strdup(yytext); return SYMBOL; }

"{"  { return LEFT_BRACE; }
"}"  { return RIGHT_BRACE; }
"("  { return '('; }
")"  { return ')'; }
";"  { return ';'; }
","  { return ','; }
":"  { return ':'; }

"+"  { return '+'; }
"-"  { return '-'; }
"*"  { return '*'; }
"/"  { return '/'; }
"="  { return '='; }

"<<" { return SHIFT_LEFT; }
">>" { return SHIFT_RIGHT; }

\n             { ++line_num; }

%%

int yywrap() { return 1; }
