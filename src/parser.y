%{

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <command.h>
#include <parser.h>
#include <scanner.h>

void yyerror(struct command* command, yyscan_t scanner, const char* msg) {
	(void)command;
	(void)scanner;
	fprintf(stderr, "\033[93mSyntax Error: %s\033[0m\n", msg);
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

// structure:
// command <args> [< input_redirect] (| command <args>)* [> output_redirect] [&]

%%
cmd_line
	: EXIT		{ /* keep the program pointer NULL to signal exit */ }
	| command input_redir pipeline output_redir back_ground
	;

back_ground
	: BACKGROUND	{ command->background = true; }
        |		{ /* nothing to be done -> background defaults to `false` */ }
        ;

command
	: command STRING	{ if (!add_argument(command, $2)) return 2; }
	| STRING		{
			if (!append_invocation(command, $1)) return 2;
			if (!add_argument(command, $1)) return 2;
		}
        ;

output_redir
	: OUTPUT_REDIR STRING	{ command->output_redir = $2; }
        | 			{ /* nothing to be done */ }
        ;

input_redir
	: INPUT_REDIR STRING	{ command->input_redir = $2; }
        |			{ /* nothing to be done */ }
        ;

pipeline
	: pipeline PIPE command	{ /* nothing to be done */ }
        |	                { /* nothing to be done */ }
        ;
%%
