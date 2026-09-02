/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_workbench/test_admin_model.c
 *
 * PURPOSE:
 *   Verify safe Data Server administration snapshots.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/data/workbench/admin_model.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDataServer *server = NULL;
    UmiDataAdminModel model;
    assert(umi_data_server_create_memory(&server) == UMI_STATUS_OK);
    assert(umi_data_server_set(server, "key", "value") == UMI_STATUS_OK);
    umi_data_admin_model_init(&model);
    assert(umi_data_admin_model_refresh(&model, server, NULL) == UMI_STATUS_OK);
    assert(model.server.record_count == 1U);
    assert(model.healthy);
    umi_data_server_destroy(server);
    return 0;
}
