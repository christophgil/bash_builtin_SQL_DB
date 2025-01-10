# bash_builtin_SQL_DB

Bash extensions for Postgresql and SQLite SQL databases in Bash scripts.








# Summary

Bash is the most widely used UNIX shell.  Using SQL databases intensively in Bash scripts is slow
because there is no integrated command for accessing SQL databases as in other programming
languages.

Since the CLI programs /usr/bin/psql or /usr/bin/sqlite3 are called for each query, there is a large
overhead.

cg_sqlite and cg_psql are fast bash-builtins for SQLite and Postgresql with less overhead.

**Status**:
Needs more testing.


# Usage

    db=/tmp/my_test_sqlite3.db
    result=''
    cg_sqlite  -$  -D <database file>  'SQL_Statements;'; aa=("${RETVAL[@]}")

    echo "Results: ${#aa[@]}    First: ${aa[0]:-}   Second: ${aa[1]:-}"

## Options

    -D  <database file or connection info, respectively>

    -$                            Store query results in the array variable 'RETVAL' instead of printing to stdout.

    -d  $'\t\n'                   Delimiter of query result for columns (1st character) and rows (optional 2nd character)
                                  Consider vertical bar as column separator: -d '|'",

    -l  <Max number of results>   Default value for stdout: Unlimited.  Default value for results stored in an array: 1024

    -V                            Print version.  Can be used to check available of the builtin

    -v                            Increase verbosity. Can be repeated.


# Example


    db=/tmp/my_test_sqlite3.db
    cg_sqlite  -D $db  'CREATE TABLE IF NOT EXISTS tbl (id int,t TEXT);'
    cg_sqlite  -D $db  "INSERT INTO tbl(id,t) VALUES($RANDOM,'$(date)');"
    cg_sqlite  -D $db  'SELECT * FROM tbl;'  # Result to stdout

    cg_sqlite  -D $db  -$  'SELECT * FROM tbl;'; array_variable=("${RETVAL[@]}")
    echo "${array_variable[@]}"



## Benchmarks

Run SQL_benchmark.sh without parameter for  instructions.

10,000 simple SELECT queries - lower values are better:

    /usr/bin/pgsql          811 seconds
    /usr/bin/sqlite          59 seconds

    cg_psql   bashbuiltin   1.3 seconds
    cg_sqlite bashbuiltin   0.5 seconds








# Compilation and Installation

First [Install dependencies](./INSTALL_DEPENDENCIES.md)

The enclosed compile script generates shared object files with the ending *.so*.

    compile_C.sh bashbuiltin_psql.c

or

    compile_C.sh bashbuiltin_sqlite.c


Builtins need to be loaded into the Bash instance with *enable*.  The following commands can be run in the
current interactive shell or can be placed at the top of a Bash script.

    enable -f ~/compiled/bashbuiltin_sqlite.so   cg_sqlite

or
    enable -f ~/compiled/bashbuiltin_psql.so   cg_psql


When they are loaded successfully, the *help* command will print the full documentation

    help cg_psql

or

    help cg_sqlite



# Limitations

Other DBs like  MySQL and  DB2 are not (yet) supported. You can send a request-for-feature.

# Related:

  - https://github.com/Aloxaf/zsh-sqlite
  - https://github.com/PietroFara/sqlite_lib.bash
  - https://github.com/koffiebaard/bashql
  - https://github.com/cjungmann/bash_sqlite
  - https://github.com/axelhahn/bash_sqlite
  - https://github.com/edean11/BashDB
