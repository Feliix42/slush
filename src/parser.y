// structure:
// command <args> [< input_redirect] (| command <args>)* [> output_redirect] [&]
// TODO: builtin befehle als enum anstatt command?
%{

#include <stdio.h>
#include <stdlib.h>
#include <command.h>
#include <parser.h>
#include <scanner.h>


int yyerror(struct command** commands, yyscan_t scanner, const char *msg) {
	fprintf(stderr, "Error: %s\n", msg);
}

%}

%code requires {
	typedef void* yyscan_t;
}

// define a reentrant parser
%define parse.error verbose
%define api.pure
%lex-param   { yyscan_t scanner }
%parse-param { struct command** commands }
%parse-param { yyscan_t scanner }


%union {
	char *string;
}

%start cmd_line
%token EXIT BACKGROUND
%token <string> PIPE INPUT_REDIR OUTPUT_REDIR STRING


%%
cmd_line    :
        | EXIT             { printf("byebye\n"); }
        | pipeline back_ground
        ;

back_ground : BACKGROUND        {  }
        |                       {  }
        ;

simple      : command redir
        ;

command     : command STRING
                {
                }
        | STRING
                {
                }
        ;

redir       : input_redir output_redir
        ;

output_redir:    OUTPUT_REDIR STRING
                {
                }
        |        /* empty */
				{
				}
        ;

input_redir:    INPUT_REDIR STRING
                {
                }
        |       /* empty */
                {
				}
        ;

pipeline    : pipeline PIPE simple
                {
                }
        | simple
                {
                }
        ;
%%
