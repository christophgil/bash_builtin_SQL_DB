#include <sqlite3.h>
#define TYPE_DB_CON sqlite3
#define NAME cg_sqlite
#define DOC_DEPENDENCIES "sqlite3-dev"
#define DOC_DB_TEST "/tmp/my_sqlite3_test.db"
#define DOC_F_OPTION "     -f  <path-of-sqlite3 database>  The db file and parent directories will be created if not yet exists."

#include "bashbuiltin_databases.h"
#define DOC_STAND_ALONE_PROGRAM "sqlite3"
#define DOC_DB_NAME_OR_FILE "database file"
#define DOC_OPTION_D "Existing or not yet existing database file. If an absolute path is provided, non-existing  parent folders will be created."


#include <sys/stat.h>
#include <errno.h>
static bool cg_mk_parent_dir(const struct struct_parameters *p){
  char rp[PATH_MAX+1];
  if (*p->db=='/'){
    strcpy(rp,p->db);
  }else{
    if (!getcwd(rp,PATH_MAX)){
      perror("Failed getcwd()");
      return false;
    }
    sprintf(rp+strlen(rp),"/%s",p->db);
    PRINT_DEBUG("absolute path of %s:   %s\n",p->db,rp);
  }
  const int n=strlen(rp);
  FOR(i,2,n){
    if (rp[i]=='/'){
      rp[i]=0;
      if (mkdir(rp,S_IRWXU) && errno!=EEXIST){
        PRINT_ERROR("Failed creating directory '%s'",rp);
        return false;
      }
      rp[i]='/';
    }
  }
  return true;
}
static bool cg_db_connect(const struct struct_parameters *p, struct struct_variables *v){
  if (!cg_mk_parent_dir(p)) return false;
  if (SQLITE_OK==sqlite3_open_v2(p->db,&v->connection,SQLITE_OPEN_READWRITE|SQLITE_OPEN_CREATE|SQLITE_OPEN_FULLMUTEX,0)){
    sqlite3_busy_timeout(v->connection,1<<20); /* 1e6 Millisec */
    PRINT_DEBUG(" v->connection: %p\n",v->connection);
    return true;
  }
  PRINT_ERROR("%s %s\n",sqlite3_errmsg(v->connection),p->db);
  sqlite3_free(v->connection);
  return false;
}

static bool wait_busy_sqlite3(const int retry,const int res,const struct struct_parameters *p){
  char *s=res==SQLITE_BUSY?"busy":res==SQLITE_LOCKED?"locked":NULL;
  if (!s) return false;
  usleep(1<<19);
  if (retry%10==0) PRINT_NOTE("%s is %s. Retry %d\n",p->db,s,retry);
  return true;
}

static void cg_process_sql(const struct struct_parameters *p, struct struct_variables *v){
  FOR(j,0,p->SQLs_l){
    sqlite3_stmt *stmt={0};
    int res;
    for (int retry=0; wait_busy_sqlite3(retry++,res=sqlite3_prepare_v2(v->connection,p->SQLs[j],-1,&stmt,NULL),p););
    if (res!=SQLITE_OK){
      PRINT_ERROR("sqlite3_prepare_v2(con,'%s')", sqlite3_errmsg(v->connection));
      continue;
    }
    for(int row=0;;row++){
      for (int retry=0; wait_busy_sqlite3(retry++,res=sqlite3_step(stmt),p););
      const int cols=sqlite3_column_count(stmt);
      if (res==SQLITE_DONE) break;
      FOR(is_data,!row && p->is_header?0:1,2){  /* is_data == 0 means print column names  */
        if (!cg_result_reset(p,v)) break;
        FOR(col,0,cols){
          char s_stack[256],*s=s_stack;
          int s_l=-1; /* strlen of s */
          if (!is_data){
            s=(char*)sqlite3_column_name(stmt,col);
          }else{
            switch(sqlite3_column_type(stmt,col)){
            case (SQLITE3_TEXT):   s=(char*)sqlite3_column_text(stmt,col); break; /* The memory space used to hold strings and BLOBs is freed automatically. */
            case (SQLITE_INTEGER): s_l=sprintf(s_stack,"%d", sqlite3_column_int(stmt,col)); break;
            case (SQLITE_FLOAT):   s_l=sprintf(s_stack,"%g", sqlite3_column_double(stmt,col)); break;
            default: break;
            }
          }
          if (!cg_result_append_column(col,s,s_l,p,v)){
            v->res=EXECUTION_FAILURE;
            sqlite3_finalize(stmt);
            return;
          }
        }/*col*/
        cg_result_apply(is_data?row:-1,p,v);
      }/* header or data */
    }/* Loop results*/
    sqlite3_finalize(stmt);
  }/* SQL statements */
}
static void cg_db_connection_unload(TYPE_DB_CON *connection){
  sqlite3_close(connection);
}
#define LOADING_BUILTIN_HOOK()  if (!sqlite3_threadsafe()) PRINT_ERROR("Not sqlite3_threadsafe()\n");
#include "bashbuiltin_databases.c"
