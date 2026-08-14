/* Umicom Framework Tests | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <string.h>
#include "umicom/data/query_session.h"
int main(void){UmiDataServer *s=NULL;UmiDatabaseQueryPolicy p=umi_database_query_policy_default();UmiDatabaseQuerySession q;UmiDatabaseQueryResult r;p.allow_writes=1;assert(umi_data_server_create_memory(&s)==UMI_STATUS_OK);assert(umi_database_query_session_init(&q,s,&p)==UMI_STATUS_OK);assert(umi_data_server_begin(s)==UMI_STATUS_OK);assert(umi_database_query_session_execute(&q,"SET workspace active",&r)==UMI_STATUS_OK&&r.row_count==1U);assert(umi_data_server_commit(s)==UMI_STATUS_OK);assert(umi_database_query_session_execute(&q,"GET workspace",&r)==UMI_STATUS_OK);assert(strcmp(r.preview,"active")==0);assert(umi_database_query_session_execute(&q,"LIST",&r)==UMI_STATUS_OK&&r.row_count==1U);assert(strstr(r.preview,"workspace=active")!=NULL&&q.history.count==3U);umi_data_server_destroy(s);return 0;}
