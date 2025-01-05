# bash_builtin_SQL_DB

NAME
====

**cg_psql**  and **cg_sqlite** — Bash builtins for using Postgresql and SQLite SQL databases in Bash scripts.

SYNOPSIS
========


    db=/tmp/my_test_sqlite3.db
    result=''
    cg_sqlite  -\> result  -D <database file>  'SQL_Statements;'
    echo "Results: ${#result[@]}    First: ${#result[0]}"



DESCRIPTION
===========

## Summary

Bash is the default command line interface for Apple MacOSX, Linux, OpenSolaris and several BSDs.
Using SQL databases intensively in Bash scripts is slow because there is no
integrated driver as in other programming languages.

The CLI programs /usr/bin/psql or /usr/bin/sqlite3 are called for each query. There is a large
overhead. External programs  are started to establish a transient connection to
the database. Less delay  comes from  capturing the output which involves the system call fork().

cg_sqlite and cg_psql are fast bash-builtins for SQLite and Postgresql with less overhead.


Please send a request-for-feature if you need a Bash builtin for other database systems like MySQL.

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



## Installation of dependencies

Install  software packages:

       - gcc or clang or  build-essential
       - bash-builtins
       - The respective database libraries and header files
          + libpq-dev for cg_psql (i.e. Postgresql)
          + sqlite3-dev for cg_sqlite
### Ubuntu or Debian Linux:

     apt-get install build-essential bash-builtins

     apt-get install postgresql libpq-dev
            or
     apt-get install sqlite3 sqlite3-dev




### MacOSX
    sudo port selfupdate

    sudo port install  bash

    port  search  postgresql | grep '(databases)' # Find out package name here postgresql96
    port install  postgresql94  libpqxx      # Take highest number. I do not know how to say install latest
          or
    port install  sqlite3


Important: The builtins will work with  the bash version installed by Mac ports  /opt/local/bin/bash, but not with  /bin/bash.
You might need to change to the newly installed Bash.



## Compilation and Installation

The enclosed compile script generates shared object files with the ending *.so*.

    compile_C.sh bashbuiltin_cg_psql.c

or

    compile_C.sh bashbuiltin_cg_sqlite.c


Builtins need to be loaded into the Bash instance with *enable*.  The following commands can be run in the
current interactive shell or can be placed at the top of a Bash script.

    enable -f ~/compiled/bashbuiltin_cg_sqlite.so   cg_sqlite

or
    enable -f ~/compiled/bashbuiltin_cg_psql.so   cg_psql


When they are loaded successfully, the *help* command will print the full documentation

    help cg_psql

or

    help cg_sqlite




## Options

    -D  <database file or connection info, respectively>

    ->  <Name of variable>        Store query results in the array variable.
                                  The '>' sign needs to be quoted with a backslash

    -d  $'\t\n'                   Delimiter of query result for columns (1st character) and rows (optional 2nd character)
                                  Consider vertical bar as column separator: -d '|'",

    -l  <Max number of results>   Default value for stdout: Unlimited.  Default value for results stored in an array: 1024

    -1                            Print the first result only or store it in a plain SHELL variable rather than an array variable
                                  Best combined with the SQL clause 'LIMIT 1'

    -V                            Print version.  Can be used to check available of the builtin

    -v                            Increase verbosity. Can occur multiple times
