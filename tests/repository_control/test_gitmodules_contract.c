/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_control/test_gitmodules_contract.c
 *
 * PURPOSE:
 *   Regression coverage for repository gitmodules contract semantics.
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
#include "umicom/repository/gitmodules.h"

int main(void)
{
    UmiRepositoryInventory *i = calloc(1U, sizeof(*i));
    assert(i != NULL);
    assert(umi_repository_gitmodules_parse("[submodule \"framework\"]\n path = framework\n url = https://example.invalid/framework.git\n branch = main\n[submodule \"studio\"]\n path = applications/studio\n url = https://example.invalid/studio.git\n", i) == UMI_STATUS_OK);
    assert(i->count == 2U);
    assert(umi_repository_inventory_find_path(i, "applications/studio") != NULL);
    free(i);
    return 0;
}
