/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime_inventory_invalid_arguments.c
 *
 * PURPOSE:
 *   Verify deterministic invalid-argument behaviour for runtime inventory
 *   helpers so diagnostics and applications never depend on undefined queries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/inventory.h"

#include <assert.h>

int main(void)
{
    size_t count = 999U;

    assert(!umi_runtime_inventory_has_command(NULL, "x"));
    assert(!umi_runtime_inventory_has_service(NULL, "x"));
    assert(!umi_runtime_inventory_has_capability(NULL, "x"));
    assert(umi_runtime_inventory_count_command_prefix(NULL, "x", &count) ==
           UMI_STATUS_INVALID_ARGUMENT);
    assert(umi_runtime_inventory_count_service_prefix(NULL, "x", &count) ==
           UMI_STATUS_INVALID_ARGUMENT);
    assert(umi_runtime_inventory_count_capability_prefix(NULL, "x", &count) ==
           UMI_STATUS_INVALID_ARGUMENT);
    assert(umi_runtime_inventory_snapshot(NULL, NULL, NULL, NULL) ==
           UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}
