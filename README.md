# bash_builtin_SQL_DB

NAME
====

**cg_psql**  and **cg_sqlite** — Bash builtins for using Postgres and SQLite SQL databases in Bash scripts.

SYNOPSIS
========


    db=/tmp/my_test_sqlite3.db
    cg_sqlite  -\>variable-for-result  -D <database file>  'SQL_Statements;'




DESCRIPTION
===========

## Summary

Bash is the default command line interface for Apple MacOSX, Linux, OpenSolaris and several BSDs.
Using SQL databases intensively in Bash scripts is slow because there is no
integrated driver as in other programming languages.

The CLI programs /usr/bin/psql or /usr/bin/sqlite3 are called for each query. There is a large
overhead. External programs  are started to establish a transient connection to
the database. Less delay  comes from  capturing the output which involves the system call fork().

cg_sqlite and cg_psql are fast bash-builtins for SQLite and Postgres with less overhead.


Please send a request-for-feature if you need a Bash builin for other database systems like Mysql.

## Status

It seems to work, but needs more testing.


## Benchmarks


Run SQL_benchmark.sh without parameter for  instructions.

10,000 simple SELECT queries - lower values are better:

    /usr/bin/pgsql          811 seconds
    /usr/bin/sqlite          59 seconds

    cg_psql   bashbuiltin   1.3 seconds
    cg_sqlite bashbuiltin   0.5 seconds




## Example 1


    db=/tmp/my_test_sqlite3.db
    cg_sqlite  -D $db  'CREATE TABLE IF NOT EXISTS tbl (id int,t TEXT);'
    cg_sqlite  -D $db  "INSERT INTO tbl(id,t) VALUES($RANDOM,'$(date)');"
    cg_sqlite  -D $db  'SELECT * FROM tbl;'  # Result to stdout

    cg_sqlite  -D $db  -\> array_variable 'SELECT * FROM tbl;'
    echo "${array_variable[@]}"

    cg_sqlite  -D $db  -\> plain_variable -1  'SELECT * FROM tbl;'
    echo "$plain_variable";



## Installation

Install  software packages:

       - gcc or clang or  build-essential
       - bash-builtins
       - The respective database libraries and header files
          + libpq-dev for cg_psql (i.e. Postgres)
          + sqlite3-dev for cg_sqlite



The enclosed compile script generates shared object files with the ending ".so".
The script compile_C.sh  works for Linux. For other operation systems, the instructions and the script will  be adapted very soon.


    compile_C.sh bashbuiltin_cg_psql.c

or

    compile_C.sh bashbuiltin_cg_sqlite.c


Before the builtins can be used, the so files  need to be loaded.
At the beginning of a script there should be a line like

    enable -f ~/compiled/bashbuiltin_cg_sqlite.so   cg_sqlite

or

    enable -f ~/compiled/bashbuiltin_cg_psql.so   cg_psql


A full documentation is printed with

    help cg_psql

or

    help cg_sqlite




## Options

    -D  <database file or connection info, respectively>

    ->  <Name of variable>        Store query results in the array variable.
                                  The '>' sign needs to be quoted with a backslash

    -d  $'\t\n'                   Delimiter of query result for columns (1st character) and rows (optional 2nd character)
                                  Consider vertical bar as column seperator: -d '|'",

    -l  <Max number of results>   Default value for stdout: Unlimited.  Default value for results stored in an array: 1024

    -1                            Print the first result only or store it in a plain SHELL variable rather than an array variable
                                  Best combined with the SQL clause 'LIMIT 1'

    -V                            Print version.  Can be used to check available of the builtin

    -v                            Increase verbosity. Can occure multiple times
