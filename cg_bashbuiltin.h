/* Author: Christoph Gille   Licence: GNU */
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#define ANSI_RESET "\x1B[0m"
#define ANSI_RED "\x1B[41m"
#define ANSI_FG_BLUE "\x1B[34;1m"
#define ANSI_FG_GREEN "\x1B[32m"
#define ANSI_FG_RED "\x1B[31m"
#define ANSI_FG_MAGENTA "\x1B[35m"
#define ANSI_FG_GRAY "\x1B[30;1m"
#define ANSI_INVERSE "\x1B[7m"
#define RED_ERROR ANSI_RED" Error "ANSI_RESET

#define FOR(var,from,to) for(int var=from;var<(to);var++)
#define RLOOP(var,from) for(int var=from;--var>=0;)
#define STRINGIZE(x) STRINGIZE_INNER(x)
#define STRINGIZE_INNER(x) #x
#define NAMEQ  STRINGIZE(NAME)
#define CONCAT(a, b) CONCAT_INNER(a, b)
#define CONCAT_INNER(a, b) a ## b

#ifndef __FILE_NAME__
#define __FILE_NAME__ __FILE__
#endif


#define VARNAME_RETVAL "RETVAL"
#define RETURN_FAIL() { if (!interactive_shell) exit_shell(EXECUTION_FAILURE); return EXECUTION_FAILURE;}


#define set_var_retval_invisible() do { SHELL_VAR *vs=find_variable(VARNAME_RETVAL); if (vs) VSETATTR(vs,att_invisible);} while(0)
