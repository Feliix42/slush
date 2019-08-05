%{

#include <string.h>
#include <command.h>
#include <parser.h>

#ifdef DOESNT_HAVE_STRDUP
#warning DOESNT_HAVE_STRDUP
char *strdup(const char *s);
#endif

%}

%option warn nodefault

%option reentrant noyywrap never-interactive nounistd noinput nounput
%option bison-bridge

NL              [\n]
PIPE            [|]
OUTPUT_REDIR    [>]
INPUT_REDIR     [<]
BG              [&]
CHAR            [0-9a-zA-Z_/\-\*\.]
SPACE           [\032]

%x IN_STRING

%%

exit            { return EXIT; }
{NL}            { return 0; }
{BG}            { return BACKGROUND; }
{PIPE}          { return PIPE; }
{OUTPUT_REDIR}  { return OUTPUT_REDIR; }
{INPUT_REDIR}   { return INPUT_REDIR; }
{CHAR}+         { yylval->string = strdup(yytext); return STRING; }

\"              { BEGIN(IN_STRING); }
<IN_STRING>\"   { BEGIN(INITIAL); }
<IN_STRING>[^"\\]*   { yylval->string = strdup(yytext); return STRING; }

.               { continue; }

%%
