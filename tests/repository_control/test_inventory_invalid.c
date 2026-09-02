/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_control/test_inventory_invalid.c
 *
 * PURPOSE:
 *   Regression coverage for repository inventory invalid semantics.
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
    UmiRepositorySubmodule s;
    assert(i != NULL); umi_repository_inventory_init(i);
    assert(umi_repository_submodule_init(&s, "framework", "framework", "", "main", 1) == UMI_STATUS_OK);
    assert(umi_repository_inventory_add(i, &s) == UMI_STATUS_OK);
    assert(umi_repository_inventory_add(i, &s) == UMI_STATUS_INVALID_STATE);
    free(i);
    return 0;
}
