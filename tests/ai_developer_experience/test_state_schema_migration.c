/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_developer_experience/test_state_schema_migration.c
 *
 * PURPOSE:
 *   Focused durable-state coverage for AI Developer Experience state schema migration.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/ai_developer_experience/state_migration.h"

int main(void)
{
    UmiSessionStore *store = NULL;
    int migrated = 1;

    assert(umi_session_store_create(&store) == UMI_STATUS_OK);
    assert(umi_ai_developer_state_schema_version() == 1U);
    assert(umi_ai_developer_state_migrate(
        store, "ai.developer", 1U, 1U, &migrated) == UMI_STATUS_OK);
    assert(migrated == 0);
    assert(umi_ai_developer_state_migrate(
        store, "ai.developer", 1U, 2U, &migrated) ==
        UMI_STATUS_NOT_IMPLEMENTED);

    umi_session_store_destroy(store);
    return 0;
}

