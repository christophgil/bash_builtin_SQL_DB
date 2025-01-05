#!/usr/bin/env bash
set -u

#  DB_NAME=''  # Please uncomment and  set!!!!
main(){
    if [[ "$#" != 2 ]]; then
        cat << EOF
 # For postgres you need to set up the database backend.
   Then set the variable DB_NAME

 # prepare a test table with the following commands:

    $0 L 0  # Prepare sqlite3 DB
    $0 P 0  # Prepare Postgres DB

# Run the benchmark tests:

       DB_NAME='name of db'  time $0 P X   # /usr/bin/psql
       DB_NAME='name of db'  time $0 P I   # bashbuiltin cg_psql
       time $0 L X                         # /usr/bin/sqlite3
       time $0 L I                         # bashbuiltin cg_sqlite
EOF
        return
    fi

    case $1 in
        P) enable -f ~cgille/compiled/bashbuiltin_cg_psql.so   cg_psql || read -r -p "Enter"

 [[ -z ${DB_NAME:-} ]] && echo 'Error: Please create a Postgres Database and set the shell variable DB_NAME at the top' && return

           x_access(){
               result="$(psql -qAX -d $DB_NAME <<< "$1")"
           }
           i_access(){
               cg_psql -\> result -D "dbname=$DB_NAME" "$1"
           }
           ;;
        L) enable -f ~/compiled/bashbuiltin_cg_sqlite.so   cg_sqlite || read -r -p "Enter"
           DB=/tmp/test_benchmark_sqlite3.db
           x_access(){
               result="$(sqlite3 $DB  "$1")"
           }
           i_access(){
               cg_sqlite -\> result -D $DB "$1"
           }
           ;;
        *) echo "Parameter 1 must be 'L' or 'P'";;
    esac

    case $2 in
        0)
            x_access 'DROP TABLE IF EXISTS test1;'
            x_access 'CREATE TABLE IF NOT EXISTS test1 (i int,square int);'
            x_access 'CREATE INDEX IF NOT EXISTS idx_test1 ON test1(i);'
            x_access 'DELETE FROM test1;'
            for ((i=0;i<10;i++)); do
                x_access "INSERT INTO test1(i,square) VALUES($i,$((i*i)) );"
            done
            return
            ;;
        I) cmd='i_access';;
        X) cmd='x_access';;
    esac

    for((i=0;i<10000;i++)); do
        $cmd "SELECT * FROM test1 WHERE i=3;"
        ((i==0)) &&  echo "$i result: $result"
        ((i%10==0)) && echo -n .
    done
}
main "$@"
