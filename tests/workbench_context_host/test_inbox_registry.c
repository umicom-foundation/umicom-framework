/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_host/test_inbox_registry.c
 *
 * PURPOSE:
 *   Verify endpoint-addressable inbox creation and removal.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "test_support.h"

int main(void)
{

    UmiWorkbenchContextHostInboxRegistry registry;
    UmiWorkbenchContextHostInbox *inbox = NULL;
    umi_workbench_context_host_inbox_registry_init(&registry);
    assert(umi_workbench_context_host_inbox_registry_ensure(
        &registry, "endpoint", "panel", &inbox) == UMI_STATUS_OK);
    assert(inbox != NULL);
    assert(registry.count == 1U);
    assert(umi_workbench_context_host_inbox_registry_find(
        &registry, "endpoint") == inbox);
    assert(umi_workbench_context_host_inbox_registry_remove(
        &registry, "endpoint") == UMI_STATUS_OK);
    assert(registry.count == 0U);
    umi_workbench_context_host_inbox_registry_destroy(&registry);
    return 0;
}
