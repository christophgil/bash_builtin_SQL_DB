#include <libpq-fe.h>

#define TYPE_DB_CON PGconn
#define NAME cg_psql
#include "bashbuiltin_cg_databases_early_inc.c"
#define DOC_DEPENDENCIES "libpq-dev"
#define DOC_STAND_ALONE_PROGRAM "psql"
#define DOC_DB_TEST "dbname=my_db"
#define DOC_OPTION_D "Example: 'dbname=my_db user=x password=xxx  host=localhost port=5432'"
#define DOC_DB_NAME_OR_FILE "database connection string"

static void cg_db_connect(const struct struct_parameters *p, struct struct_variables *v){
  v->connection=PQconnectdb(p->db);
  char *s;
#define C(c) case c: s=#c;break;
  switch(PQstatus(v->connection)){
  case CONNECTION_OK: return;
    C(CONNECTION_BAD);
    C(CONNECTION_STARTED);
    C(CONNECTION_MADE);
    C(CONNECTION_AWAITING_RESPONSE);
    C(CONNECTION_AUTH_OK);
    C(CONNECTION_SSL_STARTUP);
    C(CONNECTION_SETENV);
  default: s="Unknown";
  }
#undef C
  PRINT_ERROR("PQconnectdb %s\n",s);
  v->connection=NULL;
}
static void cg_process_sql(const struct struct_parameters *p, struct struct_variables *v){
  FOR(j,0,p->SQLs_l){
    PGresult *result=PQexec(v->connection,p->SQLs[j]);
    ExecStatusType resStatus=PQresultStatus(result);
    if (resStatus!=PGRES_TUPLES_OK) {
      PRINT_ERROR("Error while executing the query: %s\n", PQerrorMessage(v->connection));
    }else{
      const int rows=PQntuples(result), cols=PQnfields(result);
      FOR(row,(p->is_header?-1:0),rows){
        if (!cg_result_reset(p,v)) break;
        FOR(col,0,cols){
          const char *s=row<0?PQfname(result,col):PQgetvalue(result,row,col);
          if (!cg_result_append_column(col,s,-1,p,v)){ v->res=EXECUTION_FAILURE;PQclear(result);return;}
        }/*col*/
        cg_result_appy(row,p,v);
      }/*row*/
    }/*PGRES_TUPLES_OK*/
    if (result) PQclear(result);
  }/*SQLs*/
}
static void cg_db_connection_unload(TYPE_DB_CON *connection){
  PQfinish(connection);
}
#include "bashbuiltin_cg_databases_inc.c"
