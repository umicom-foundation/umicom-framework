/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_workbench/test_runtime.c
 *
 * PURPOSE:
 *   Verify the unified runtime executes through the canonical query session.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <stdlib.h>

#include "umicom/data/workbench/runtime.h"

int main(void)
{
    UmiDataServer *server = NULL;
    UmiDatabaseExplorer *explorer = NULL;
    UmiDataWorkbenchRuntime *runtime = NULL;
    UmiDataWorkbenchRuntimeSnapshot snapshot;
    UmiDataSqlDocument *sql;
    assert(umi_data_server_create_memory(&server) == UMI_STATUS_OK);
    assert(umi_data_server_set(server, "alpha", "one") == UMI_STATUS_OK);
    explorer = (UmiDatabaseExplorer *)calloc(1U, sizeof(*explorer));
    assert(explorer != NULL);
    assert(umi_database_explorer_init(explorer, server) == UMI_STATUS_OK);
    assert(umi_data_workbench_runtime_create(explorer, &runtime) ==
           UMI_STATUS_OK);
    assert(umi_data_workbench_runtime_open_sql(
               runtime, "sql-1", "List", "umicom-data") == UMI_STATUS_OK);
    sql = umi_data_workbench_runtime_sql(runtime);
    assert(umi_data_sql_document_set_text(sql, "LIST", sql->revision) ==
           UMI_STATUS_OK);
    assert(umi_data_workbench_runtime_execute(runtime) == UMI_STATUS_OK);
    assert(umi_data_workbench_runtime_snapshot(runtime, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.phase == UMI_DATA_WORKBENCH_SHOWING_RESULTS);
    assert(snapshot.last_status == UMI_STATUS_OK);
    umi_data_workbench_runtime_destroy(runtime);
    free(explorer);
    umi_data_server_destroy(server);
    return 0;
}
