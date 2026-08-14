/* Umicom Framework Tests | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "umicom/data/query_session.h"

int main(void)
{
    UmiDataServer *server = NULL;
    UmiDatabaseQueryPolicy policy = umi_database_query_policy_default();
    UmiDatabaseQuerySession *session;
    UmiDatabaseQueryResult result;

    session = (UmiDatabaseQuerySession *)malloc(sizeof(*session));
    assert(session != NULL);
    policy.allow_writes = 1;
    assert(umi_data_server_create_memory(&server) == UMI_STATUS_OK);
    assert(umi_database_query_session_init(session, server, &policy) == UMI_STATUS_OK);
    assert(umi_data_server_begin(server) == UMI_STATUS_OK);
    assert(umi_database_query_session_execute(session, "SET workspace active", &result) == UMI_STATUS_OK);
    assert(result.row_count == 1U);
    assert(umi_data_server_commit(server) == UMI_STATUS_OK);
    assert(umi_database_query_session_execute(session, "GET workspace", &result) == UMI_STATUS_OK);
    assert(strcmp(result.preview, "active") == 0);
    assert(umi_database_query_session_execute(session, "LIST", &result) == UMI_STATUS_OK);
    assert(result.row_count == 1U);
    assert(strstr(result.preview, "workspace=active") != NULL);
    assert(session->history.count == 3U);
    umi_data_server_destroy(server);
    free(session);
    return 0;
}
