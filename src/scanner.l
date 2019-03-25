%{
#include <string.h>
#include <command.h>
#include <parser.h>

#ifdef DOESNT_HAVE_STRDUP
#warning DOESNT_HAVE_STRDUP
char *strdup(const char *s);
#endif

#define ECHO
%}

%option warn nodefault

%option reentrant noyywrap never-interactive nounistd
%option bison-bridge

NL              [\n]
PIPE            [|]
OUTPUT_REDIR    [>]
INPUT_REDIR     [<]
BG              [&]
CHAR            [0-9a-zA-Z_/\-\*\.]
SPACE           [\032]

%%

exit            { ECHO; return EXIT; }
{NL}            { ECHO; return 0; }
{BG}            { ECHO; return BACKGROUND; }
{PIPE}          { ECHO; return PIPE;}
{OUTPUT_REDIR}  { ECHO; return OUTPUT_REDIR; }
{INPUT_REDIR}   { ECHO; return INPUT_REDIR; }
{CHAR}+         { ECHO; yylval->string = strdup(yytext); return STRING; }
.               { continue; }

%%

//void yyerror(const char* msg) {
//	// TODO: Can this be improved for better error handling?
//	fprintf(stderr, "Error: %s\n", msg);
//}
