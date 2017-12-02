%{
/*
 *                                                   _
         .-==-.                   ..--/\/\--..      (_)
    __      ___    __      ____  ___  ____  ___     __ __
   /  \  /\/  _\/\/  \  /\/ _  \/ _ \/ _  \/ _ \/\ / //  \  /\
  / /\ \/ /  (_) / /\ \/ / (_) / (_)/ (_) / (_)  // // /\ \/ /
  \/  \__/\__/\_/\/  \__/\____/ (_) \ __ (\__/\_//_/ \/  \__/
                              \_____//  \_\
   .-==-.           --===/\/\/\===---             ..--/\/\--..

    NanoBrain C Compiler (c) 2017 James Ross Sharp

                                                                            */

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
[ \t]          ;
\/\/    { BEGIN(COMMENT); }
<COMMENT>\n { ++line_num; BEGIN(INITIAL); return ENDL; }
<COMMENT>. ;

\"  { g_str_idx = 0; BEGIN(STRING_LITERAL); }
<STRING_LITERAL>\n { ++line_num; }
<STRING_LITERAL>\" { BEGIN(INITIAL); g_string[g_str_idx] = '\0'; yylval.stringLiteral = strdup(g_string); return STR_LITERAL; }
<STRING_LITERAL>. { if (g_str_idx < MAX_STRING - 2) g_string[g_str_idx++] = yytext[0];};

\n             { ++line_num; return ENDL; }

%%

int yywrap() { return 1; }
