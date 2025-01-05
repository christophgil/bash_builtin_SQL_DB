#!/usr/bin/env bash
set -u
export ANSI_RED=$'\e[41m' ANSI_MAGENTA=$'\e[45m' ANSI_GREEN=$'\e[42m' ANSI_BLUE=$'\e[44m'  ANSI_YELLOW=$'\e[43m' ANSI_WHITE=$'\e[47m' ANSI_BLACK=$'\e[40m'
export ANSI_FG_GREEN=$'\e[32m' ANSI_FG_RED=$'\e[31m' ANSI_FG_MAGENTA=$'\e[35m' ANSI_FG_GRAY=$'\e[30;1m' ANSI_FG_BLUE=$'\e[34;1m' ANSI_FG_BLACK=$'\e[100;1m' ANSI_FG_YELLOW=$'\e[33m' ANSI_FG_WHITE=$'\e[37m'
export ANSI_INVERSE=$'\e[7m' ANSI_BOLD=$'\e[1m' ANSI_UNDERLINE=$'\e[4m' ANSI_RESET=$'\e[0m'
CCOMPILER=clang
! clang --version |head -n 1 && CCOMPILER=gcc
dirpaths_with_pfx(){
    local pfx=$1 d
    shift
    for d in "$@"; do [[ -d $d ]] && echo -n "$pfx$d"; done
}
main(){
    echo $ANSI_INVERSE"This is $0"$ANSI_RESET
    local src=${1:-} is_bashbuiltin=0
    [[ -z $src ]] && echo -e "Compile a file with gcc or clang.\nUsage: ${ANSI_BLACK}${ANSI_FG_GREEN}$0  my-source-file.c$ANSI_RESET" && return 1
    local includes="$(grep '^#include ' $src)"
    [[ /$src == */bashbuiltin_*.c ||
           $includes == *\"loadables.h\"* ||
           $includes == *\"bashtypes.h\"* ||
           $includes == *\"bashgetopt.h\"* ]] && echo "Compiling a bash builtin" && is_bashbuiltin=1
        local as="-O0 -fsanitize=address -fno-omit-frame-pointer"
    SEARCH_INCLUDE_FILES='/usr/include/postgresql  /usr/lib /opt/local/include /opt/local/lib'
    ((is_bashbuiltin)) && as='' && SEARCH_INCLUDE_FILES+=' /usr/lib/bash /usr/include/bash /usr/include/bash/include /usr/include/bash/builtins \
     /opt/local/include/bash /opt/local/include/bash/include /opt/local/include/bash/builtins '
    if [[ $includes == *\<libpq-fe.h\>* ]]; then
        local inc=$(find /opt/local/include/postgresql* -name libpq-fe.h 2>/dev/null)
        SEARCH_INCLUDE_FILES+=" ${inc%/*} "
    fi
    IPATHS=$(dirpaths_with_pfx ' -I' $SEARCH_INCLUDE_FILES)
    export ASAN_OPTIONS=quarantine_size_mb=1
    export PATH=/usr/lib/llvm-20/bin/:$PATH
    export PATH=/usr/lib/llvm-14/bin/:$PATH
    $CCOMPILER --version
    # -pedantic-errors
    local libs='' stack='-fno-stack-protector'
    [[ ${OSTYPE,,} == linux* ]] && stack+=' -z execstack'


    [[ $includes == *\<sqlite3.h\>* ]] && libs+=' -lsqlite3 '
    [[ $includes == *\<pthread.h\>* ]] && libs+=' -lpthread '
    [[ $includes == *\<libpq-fe.h\>* ]] && libs+=' -lpq '
    local name=${src##*/}
    name=${name%.c}
    local out=~/compiled/$name  mk_shared_object=''  is_so=is_bashbuiltin
    echo "out: $out"
    mkdir -p ${out%/*}
    [[ $name == lib* ]] && is_so=1
    ((is_so)) && out+=.so && mk_shared_object='-shared -undefined dynamic_lookup  -fPIC'
    local cmd="$CCOMPILER $mk_shared_object $stack -Wno-string-compare -Wno-strict-prototypes -g $as -rdynamic $IPATHS  $src  $libs -o $out"
    echo $cmd
    rm $out 2>/dev/null
    $cmd
    ls -l $out
    if ((is_bashbuiltin)); then
        local builtins="$(sed -n -e 's|^struct builtin \(.*\)_struct.*$|\1|p' -e 's|^MY_BASHBUILTIN(\(\w*\).*$|\1|p'  $src)"
        [[ -z $builtins ]] && builtins=$(sed -n -e 's|^#define NAME \(.*\)|\1|p' $src);

        echo "To activate the plugins in bash please install sudo ${ANSI_BLACK}${ANSI_FG_GREEN}apt-get install bash-builtins$ANSI_RESET. Then run in bash:"
        echo -e  "${ANSI_BLACK}${ANSI_FG_GREEN}bash\nbb='$(echo $builtins)'\nenable -d  \$bb 2>/dev/null; enable -f $out \$bb" $ANSI_RESET

    fi
}
main "$@"
