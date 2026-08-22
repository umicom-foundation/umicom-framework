/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_host/test_session_service.c
 *
 * PURPOSE:
 *   Verify context-host assignments persist through chunked Framework Session Store values.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "test_support.h"
#include "umicom/platform/session_store.h"
#include "umicom/workbench_context_host/session_service.h"

int main(void)
{
    UmiWorkbenchContextHostTestFixture fixture;
    UmiSessionStore *store = NULL;
    bool restored = false;

    assert(umi_wch_test_fixture_init(&fixture) == UMI_STATUS_OK);
    assert(umi_session_store_create(&store) == UMI_STATUS_OK);

    assert(umi_workbench_context_host_set_active_group(
        fixture.host, "red") == UMI_STATUS_OK);
    assert(umi_workbench_context_host_session_save(
        fixture.host, store, "test.context") == UMI_STATUS_OK);

    assert(umi_workbench_context_host_set_active_group(
        fixture.host, "blue") == UMI_STATUS_OK);
    assert(umi_workbench_context_host_session_restore(
        fixture.host, store, "test.context", &restored) == UMI_STATUS_OK);
    assert(restored);
    assert(strcmp(fixture.host->active_group_id, "red") == 0);

    assert(umi_workbench_context_host_session_remove(
        store, "test.context") == UMI_STATUS_OK);
    restored = true;
    assert(umi_workbench_context_host_session_restore(
        fixture.host, store, "test.context", &restored) == UMI_STATUS_OK);
    assert(!restored);

    umi_session_store_destroy(store);
    umi_wch_test_fixture_destroy(&fixture);
    return 0;
}
