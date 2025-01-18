#!/usr/bin/env bash
set -u
readonly ANSI_RED=$'\e[41m'
readonly ANSI_FG_GREEN=$'\e[32m'
readonly ANSI_RESET=$'\e[0m'
readonly SRC=$(realpath ${BASH_SOURCE[0]})
echo SRC=$SRC
source ${SRC%/*}/load_this_builtin.sh
readonly SRC_FNAME=${SRC##*/}
readonly DIR=~/test/${SRC_FNAME%.sh}

assert_eq(){
    local v="$2"
    [[ $v == '@' ]] && v="${RETVAL:-}"
    if [[ $1 == "$v" ]]; then
        echo "${ANSI_FG_GREEN}OK$ANSI_RESET"
    else
        echo "${ANSI_RED}WRONG RESULT$ANSI_RESET in Line ${BASH_LINENO[0]}    '$1' != '$v'"
        read -r -p "Enter"
    fi
}
