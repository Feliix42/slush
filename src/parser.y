// TODO: builtin befehle als enum anstatt command?
%{

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <command.h>
#include <parser.h>
#include <scanner.h>

struct program* current_program = NULL;

int yyerror(struct command* command, yyscan_t scanner, const char *msg) {
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
%parse-param { struct command* command }
%parse-param { yyscan_t scanner }


%union {
	char *string;
}

%start cmd_line
%token EXIT BACKGROUND
%token <string> PIPE INPUT_REDIR OUTPUT_REDIR STRING

// TODO: Reformulate grammar to only allow 1 input & output redir (maybe by changing th cmd_line rule?
 // structure:
 // command <args> [< input_redirect] (| command <args>)* [> output_redirect] [&]

%%
cmd_line
	: EXIT             { /* keep the program pointer NULL to signal exit */ }
	| command input_redir pipeline output_redir back_ground {printf("cmd_line\n");}
	;

back_ground
	: BACKGROUND	{ command->background = true; }
        |		{ /* default case */ }
        ;

command
	: command STRING	{ if (!add_argument(command, yyval.string)) return 0; }
        | STRING		{ if (!append_invocation(command, yyval.string)) return 0; }
        ;

output_redir:    OUTPUT_REDIR STRING
		{ printf("output redirection\n"); command->output_redir = yyval.string; }
        |        /* empty */
		{ /* nothing to be done */ }
        ;

input_redir:    INPUT_REDIR STRING
		{ printf("input redirection\n"); command->input_redir = yyval.string; }
        |        /* empty */
		{ /* nothing to be done */ }
        ;

pipeline    : pipeline PIPE command
		{
                }
        |        /* empty */
                {
                }
        ;
%%
