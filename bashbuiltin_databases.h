////////////////////////////////////////////////////////////////
/// Author: Christoph Gille                                  ///
/// Licence: GNU                                             ///
/// COMPILE_MAIN=bashbuiltin_psql.c                          ///
/// This contains the common part and is included by the     ///
/// specific implementations for different database backends ///
////////////////////////////////////////////////////////////////

#define _GNU_SOURCE 1

#include <getopt.h>
#include "bash/loadables.h"
#include "cg_bashbuiltin.h"

#define WITH_ASSOCARRAY 0

#define MAX_PATHLEN 1024
#define CONNECTIONS 8
#define DEFAULT_MAX_RESULTS 1024




/* After initialization this will not change */
struct struct_parameters{
  bool retvar;                  /* Assign variable RETVAL rather than writing to stdout */
    bool log_sql_to_stderr;
  int max_num_results;
  bool is_header;               /* Print a line with column names */
  char delim_col, delim_row;      /* Normally \t and \n */
  int verbose;
  char **SQLs;          /* The sql statements provided as  command line arguments */
  int SQLs_l;           /* Number of SQLs */
  char db[PATH_MAX+1];            /* Database name or in case of sqlite3 the database file */
};


/* This struct contains values that might change */
struct struct_variables{
  char *result;         /* Result of SQL query */
  int result_capacity;  /* Allocated memory */
  int result_l;         /* Current lengthe of result */
  int result_idx;       /* Count results */
        bool set_variable_retval_done; /* Already set RETVAL  */
  int res;              /* Return status. 0 is success */
  #if WITH_ASSOCARRAY
  SHELL_VAR *shell_var; /* Shell variable to store the SQL result or empty String */
  #endif
  TYPE_DB_CON *connection;
};
/*  Appending the String s to the growing result string stored in struct_variables->result */
static bool cg_result_append_column(const int column, const char *s,  int s_l, const struct struct_parameters *p,struct struct_variables *v);

/*  Must be called before appending result fields with  cg_result_append_column()  */
static bool cg_result_reset(const struct struct_parameters *p,struct struct_variables *v);


/*  Appending the String s to the growing result string stored in struct_variables->result */
static bool cg_result_apply(const int row,const struct struct_parameters *p,struct struct_variables *v);

/* Output will be generated only if query starts with 'select' (case insensitive) */
static bool cg_starts_with_select(const struct struct_parameters *p,const char* sql);
