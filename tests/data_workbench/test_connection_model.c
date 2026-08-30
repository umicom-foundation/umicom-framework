/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_workbench/test_connection_model.c
 *
 * PURPOSE:
 *   Verify connection filtering, state summaries and activation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/data/workbench/connection_model.h"

int main(void)
{
    UmiDatabaseConnectionRegistry registry;
    UmiDatabaseConnectionProfile profile = {0};
    UmiDataWorkbenchConnectionModel model;
    assert(umi_database_connection_registry_init(&registry) == UMI_STATUS_OK);
    (void)strcpy(profile.id, "primary");
    (void)strcpy(profile.display_name, "Primary SQLite");
    (void)strcpy(profile.endpoint, "data/main.db");
    profile.provider = UMI_DATABASE_PROVIDER_SQLITE;
    profile.state = UMI_DATABASE_READY;
    assert(umi_database_connection_registry_upsert(&registry, &profile) ==
           UMI_STATUS_OK);
    assert(umi_data_workbench_connection_model_init(&model, &registry) ==
           UMI_STATUS_OK);
    assert(model.ready_count == 1U);
    assert(umi_data_workbench_connection_model_set_filter(&model, "sqlite") ==
           UMI_STATUS_OK);
    assert(model.visible_count == 1U);
    assert(umi_data_workbench_connection_model_activate(&model, "primary") ==
           UMI_STATUS_OK);
    assert(strcmp(registry.active_id, "primary") == 0);
    return 0;
}
