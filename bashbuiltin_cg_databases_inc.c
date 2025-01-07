/////////////////////////////////////////////////////////////////
///  COMPILE_MAIN=bashbuiltin_cg_sqlite.c                     ///
///  This contains the common part and is included by the     ///
///  specific implementations for different database backends ///
/////////////////////////////////////////////////////////////////

#define USAGE() "    db='"DOC_DB_TEST"'\n"\
  "        "NAMEQ" -D \"$db\"  'CREATE TABLE IF NOT EXISTS tbl (id int,t TEXT);'\n"\
  "        "NAMEQ" -D \"$db\"  \"INSERT INTO tbl(id,t) VALUES($RANDOM,'$(date)');\"\n"\
  "        "NAMEQ" -D \"$db\"  'SELECT * FROM tbl;'  # Result to stdout\n"\
  "        "NAMEQ" -D \"$db\"  -\\> array_variable 'SELECT * FROM tbl;'; echo \"${array_variable[@]}\"  # Results captured in array variable\n"\
  "        "NAMEQ" -D \"$db\"  -\\> plain_variable -1  'SELECT * FROM tbl;'; echo \"$plain_variable\";  # Single result in variable\n"


static char *_doc[]={
  ANSI_INVERSE"Accessing SQL Databases in Bash scripts"ANSI_RESET, "", "SYNOPSIS",USAGE(),
  "", "SUMMARY",
  "  In Bash scripts, "NAMEQ" may replace /usr/bin/"DOC_STAND_ALONE_PROGRAM".",
  "  It is a Bash builtin. Once loaded it stays in memory. ",
  "  It allows for up to "STRINGIZE(CONNECTIONS)" persistent  database connections.",
  "  The maximum number of different databases is encoded in the macro CONNECTIONS and can be increased.",
  "  Advantages of , "NAMEQ" in scripts:",
  "      - Improved performance because there is no overhead by starting an external program and establishing a connection to a database.",
  "      - The result of queries can be captured directly in array or plain variables.",
  "", "INSTALLATION",
  "  Install packages:",
  "     - gcc or clang or  build-essential",
  "     - bash-builtins",
  "     - "DOC_DEPENDENCIES,
  "",
  "  The source needs to be compiled. The enclosed compilation script can be used:",
  "     compile_C.sh  "__FILE_NAME__,
  "",
  "  Compilation results in a shared object file with the ending .so.  The builtin is then loaded into  bash with the command",
  "     enable -f ~/compiled/file.so   "NAMEQ,
  "", "OPTIONS","",
  "     -D  <"DOC_DB_NAME_OR_FILE">   "DOC_OPTION_D,  "","",
  "     ->  <Name of variable>          Store query results in the array variable",
  "                                     The '>' sign needs to be quoted with a backslash","",
  "     -d  $'\\t\\n'                     Delimiter of query result for columns (1st character) and rows (optional 2nd character)",
  "                                     Consider vertical bar as column seperator: -d '|'","",
  "     -l  <Max number of results>     Default value: Unlimited for stdout.  "STRINGIZE(DEFAULT_MAX_RESULTS)" for results stored in an array","",
  "     -1                              Print the first result or store the first result in a plain SHELL variable rather than an array",
  "                                     Best used together with the SQL clause 'LIMIT 1'","",
  "     -V                              Print version.  Can be used to check available of the builtin","",
  "     -v                              Increase verbosity. Can be repeated","",
  (char*)NULL
};


struct cg_connection{
  TYPE_DB_CON *connection;
  char db[MAX_PATHLEN+1];
  int db_l;
};
static void db_connection_for_path(struct struct_parameters *p, struct struct_variables *v){
  static int cc_l=0;
  static struct cg_connection cc[CONNECTIONS+1]={0};
  if (!p){ /* called from xxxxx_builtin_unload() */
    FOR(i,0,CONNECTIONS){
      if (cc[i].connection){
        PRINT_NOTE("Going to close db connection %s\n",cc[i].db);
        cg_db_connection_unload(cc[i].connection);
      }
    }
    return;
  }
  const char *n=p->db;
  const int n_l=strlen(n);
  FOR(i,0,CONNECTIONS){
    if (cc[i].db_l==n_l && !strcmp(cc[i].db,n)){
      v->connection=cc[i].connection;
      return;
    }
  }
  if (cc_l<CONNECTIONS){
    cg_db_connect(p,v);
    if ((cc[cc_l].connection=v->connection)){
      strcpy(cc[cc_l].db,n);
      cc[cc_l].db_l=n_l;
    }
  }else{
    PRINT_ERROR("To many connections %d. Recompile with larger value for macro CONNECTIONS!\n",CONNECTIONS);
  }
}
/* ================================================================================ */
static int cg_db_builtin_main(const int argc, char *argv[]){
  struct struct_parameters para={.delim_col='\t', .delim_row='\n', .verbose=0 }, *p=&para;
  optind=0;
#define V() ANSI_FG_BLUE"%s"ANSI_RESET
  for (int opt; (opt=getopt(argc,argv,"HD:d:1l:vV:h>:"))!=-1;){
    switch(opt){
    case '>':
      if (sizeof(p->retvar)-1<=strlen(optarg)) RETURN_ERROR("Variable name "V()"too long",optarg);
      strcpy(p->retvar,optarg);
      if (!p->max_num_results) p->max_num_results=DEFAULT_MAX_RESULTS;
      break;
    case 'd':
      if (*optarg){
        p->delim_col=*optarg;
        if (optarg[1]) p->delim_row=optarg[1];
      }
      break;
    case 'H': p->is_header=true; break;
    case 'D': if (sizeof(p->db)-1<=strlen(optarg)) RETURN_ERROR("Option  -D '"V()"' exceeds length %lu\n",optarg,sizeof(p->db)-1);
      strcpy(p->db,optarg); break;
    case '1': p->is_single_result=true; break;
    case 'l': p->max_num_results=atoi(optarg); break;
    case 'V': PRINT_NOTE("Version 0\n"); return 0;
    case 'v': p->verbose++; PRINT_NOTE("verbose: %d\n",p->verbose); break;
    case 'h': builtin_usage(); return EX_USAGE;
    default: builtin_usage();RETURN_ERROR("Wrong option -%c\n",opt);
    }
  }
  PRINT_DEBUG("db: '"V()"'  retvar: "V()"  delim_col: %d delim_row: %d\n",p->db,p->retvar,p->delim_col,p->delim_row);
  if (!*p->db){ builtin_usage(); RETURN_ERROR("Please specify "DOC_DB_NAME_OR_FILE" with option -D\n"); }
  struct struct_variables var={.result_capacity=1024,.result=malloc(1024)}, *v=&var;
  db_connection_for_path(p,v);
  if (!v->connection){ RETURN_ERROR("Failed to connect to the database  '"V()"'\n",p->db); }
  p->SQLs=argv+optind;
  p->SQLs_l=argc-optind;
  /* From now-on no modifications of p */
  if (*p->retvar){
    if (p->is_single_result){
      if (variable_context) make_local_variable(p->retvar,0);
    }else{
      if (!(v->shell_var=find_or_make_array_variable(p->retvar,0))) RETURN_ERROR("Failed find_or_make_array_variable "V()"\n",p->retvar);
      if (readonly_p(v->shell_var) || noassign_p(v->shell_var)) RETURN_ERROR("Variable "V()" read-only or not assignable",p->retvar);
      if (!array_p(v->shell_var)) RETURN_ERROR("Not an indexed array: "V(),p->retvar);
      if (invisible_p(v->shell_var)) VUNSETATTR(v->shell_var,att_invisible);/* no longer invisible */
      array_flush(array_cell(v->shell_var));
    }
  }
#undef V
  cg_process_sql(p,v);
  free(v->result);
  return v->res;
}

static int cg_db_builtin(WORD_LIST *list){
  int argc;
  char **argv=make_builtin_argv(list, &argc);
  QUIT;
  const int r=cg_db_builtin_main(argc,argv);
  free(argv);
  return r;
}

struct builtin CONCAT(NAME,_struct)={NAMEQ,cg_db_builtin,BUILTIN_ENABLED,_doc,"\n",USAGE()};
int CONCAT(NAME,_builtin_load)(char *s){
  PRINT_NOTE("Loading builtin "ANSI_FG_BLUE""STRINGIZE(NAME)""ANSI_RESET"\n");
#ifdef LOADING_BUILTIN_HOOK
  LOADING_BUILTIN_HOOK()
#endif
    return 1;
}
void CONCAT(NAME,_builtin_unload)(char *s){
  PRINT_NOTE(ANSI_INVERSE"Unloading builtin " STRINGIZE(NAME) ""ANSI_RESET"\n");
  db_connection_for_path(NULL,NULL);
}
