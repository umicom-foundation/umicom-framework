/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_control/test_inventory_boundary.c
 *
 * PURPOSE:
 *   Regression coverage for repository inventory boundary semantics.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable repository-control capability. Applications
 *   remain thin consumers and must not duplicate this policy or state model.
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
#include <stdio.h>
#include "umicom/repository/inventory.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiRepositoryInventory *i = calloc(1U, sizeof(*i));
    UmiRepositorySubmodule s; size_t n;
    assert(i != NULL); umi_repository_inventory_init(i);
    /* Visit each bounded item once so every record receives the same rule. */
    for (n = 0U; n < UMI_REPOSITORY_CONTROL_ITEM_CAPACITY; ++n) {{
        char name[64], path[64];
        (void)snprintf(name, sizeof(name), "m%zu", n);
        (void)snprintf(path, sizeof(path), "modules/m%zu", n);
        assert(umi_repository_submodule_init(&s, name, path, "", "", 1) == UMI_STATUS_OK);
        assert(umi_repository_inventory_add(i, &s) == UMI_STATUS_OK);
    }}
    assert(umi_repository_submodule_init(&s, "overflow", "modules/overflow", "", "", 1) == UMI_STATUS_OK);
    assert(umi_repository_inventory_add(i, &s) == UMI_STATUS_CAPACITY_EXCEEDED);
    free(i);
    return 0;
}
