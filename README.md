# bash_builtin_SQL_DB

NAME
====

**cg_psql**  **cg_sqlite** — Loadable Bash builtins for using SQL databases in Bash scripts.

SYNOPSIS
========


  db=/tmp/my_test_sqlite3.db
  cg_sqlite  -\>variable-for-result  -D <database file>  'SQL_Statements;'




DESCRIPTION
===========

## Summary

Bash is the default command line interface for Apple MacOSX, Linux, OpenSolaris and several BSDs.
Within Bash scripts, the external programs /usr/bin/psql, /usr/bin/sqlite3 and /usr/bin/mysql are
typically used to access SQL database.  For each DB access, the respective  program is started and a new
connection to the database is established and  closed afterwards.  The standard output is captured and processed.

Here we provide bash-builtins for Sqlite3 and Postgres that can do the same with less overhead.
Simple queries will be orders of magnitudes  faster.

Please send a request-for-feature if you need a Bash builin for other database systems like Mysql.

## Status

It seems to work, but needs more testing.


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

         ->  <Name of variable>          The variable will hold the results. It is an array, unless  option '-1' is given. The '>' sign needs to be quoted with a backslash

         -d  $'\t\n'                     Delimiter of query result for columns and for rows. It is a string of length 1 or 2.  You might use vertical bar:  -d '|'

         -l  <Max number of results>     Default value: Unlimited for stdout.  1024 for results stored in an array.

         -1                              Return not more than one result. If the result is stored in a variable then it will be a plain variable rather than an array.

         -V                              Print version.  Can be used to check available of the builtin.

         -v                              Verbose output to stderr.
