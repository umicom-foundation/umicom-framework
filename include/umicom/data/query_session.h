/* Umicom Framework | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_DATA_QUERY_SESSION_H
#define UMICOM_DATA_QUERY_SESSION_H
#include "umicom/data/data_server.h"
#include "umicom/data/query_history.h"
#include "umicom/data/query_policy.h"
typedef struct UmiDatabaseQueryResult { UmiDatabaseQueryKind kind; UmiStatus status; size_t row_count; char preview[UMI_DATABASE_RESULT_CAPACITY]; char message[UMI_DATABASE_TEXT_CAPACITY]; } UmiDatabaseQueryResult;
typedef struct UmiDatabaseQuerySession { UmiDataServer *server; UmiDatabaseQueryPolicy policy; UmiDatabaseQueryHistory history; uint64_t revision; } UmiDatabaseQuerySession;
UmiStatus umi_database_query_session_init(UmiDatabaseQuerySession *session,UmiDataServer *server,const UmiDatabaseQueryPolicy *policy);
UmiStatus umi_database_query_session_execute(UmiDatabaseQuerySession *session,const char *statement,UmiDatabaseQueryResult *out_result);
#endif
