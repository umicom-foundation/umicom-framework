/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_host/test_session_service.c
 *
 * PURPOSE:
 *   Verify context-host assignments persist through bounded, line-safe
 *   Framework Session Store values and restore the active workbench group.
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

#include "test_support.h"
#include "umicom/platform/session_store.h"
#include "umicom/workbench_context_host/session_service.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiWorkbenchContextHostTestFixture fixture;
    UmiSessionStore *store = NULL;
    bool restored = false;
    size_t index;

    assert(umi_wch_test_fixture_init(&fixture) == UMI_STATUS_OK);
    assert(umi_session_store_create(&store) == UMI_STATUS_OK);

    assert(umi_workbench_context_host_set_active_group(
        fixture.host, "red") == UMI_STATUS_OK);

    assert(umi_workbench_context_host_session_save(
        fixture.host, store, "test.context") == UMI_STATUS_OK);

    /*
     * Session Store persists one key=value record per line. The context-host
     * serializer is multi-line, therefore every stored chunk must cross this
     * boundary in the line-safe encoded representation.
     */
    assert(umi_session_store_count(store) >= 2U);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < umi_session_store_count(store); ++index) {
        UmiSessionEntrySnapshot snapshot;

        assert(umi_session_store_at(
            store, index, &snapshot) == UMI_STATUS_OK);
        assert(strchr(snapshot.value, '\n') == NULL);
        assert(strchr(snapshot.value, '\r') == NULL);
    }

    assert(umi_workbench_context_host_set_active_group(
        fixture.host, "blue") == UMI_STATUS_OK);

    assert(umi_workbench_context_host_session_restore(
        fixture.host,
        store,
        "test.context",
        &restored) == UMI_STATUS_OK);
    assert(restored);
    assert(strcmp(fixture.host->active_group_id, "red") == 0);

    assert(umi_workbench_context_host_session_remove(
        store, "test.context") == UMI_STATUS_OK);

    restored = true;
    assert(umi_workbench_context_host_session_restore(
        fixture.host,
        store,
        "test.context",
        &restored) == UMI_STATUS_OK);
    assert(!restored);

    umi_session_store_destroy(store);
    umi_wch_test_fixture_destroy(&fixture);
    return 0;
}
