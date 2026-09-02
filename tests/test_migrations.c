/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_migrations.c
 *
 * PURPOSE:
 *   Verify migration planning, ordered application and schema-version persistence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/umicom.h"

#include <assert.h>

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDataServer *server = NULL;
    UmiMigrationRegistry *registry = NULL;
    UmiMigration one = {1U, "initial", "", ""};
    UmiMigration two = {2U, "second", "", ""};
    UmiMigrationPlan plan;
    UmiStore store;
    uint32_t version = 0U;
    size_t applied = 0U;
    assert(umi_data_server_create_memory(&server) == UMI_STATUS_OK);
    assert(umi_migration_registry_create(4U, &registry) == UMI_STATUS_OK);
    assert(umi_migration_registry_add(registry, &two) == UMI_STATUS_OK);
    assert(umi_migration_registry_add(registry, &one) == UMI_STATUS_OK);
    assert(umi_migration_plan(registry, server, 2U, &plan) == UMI_STATUS_OK);
    assert(plan.pending_count == 2U);
    assert(umi_migration_apply(registry, server, 2U, &applied) == UMI_STATUS_OK);
    assert(applied == 2U);
    assert(umi_store_from_data_server(server, &store) == UMI_STATUS_OK);
    assert(umi_schema_version_get(&store, &version) == UMI_STATUS_OK);
    assert(version == 2U);
    umi_migration_registry_destroy(registry);
    umi_data_server_destroy(server);
    return 0;
}
