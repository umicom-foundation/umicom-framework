/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_control/test_gitmodules_invalid.c
 *
 * PURPOSE:
 *   Regression coverage for repository gitmodules invalid semantics.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable repository-control capability. Applications
 *   remain thin consumers and must not duplicate this policy or state model.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <stdlib.h>
#include "umicom/repository/gitmodules.h"

int main(void)
{
    UmiRepositoryInventory *i = calloc(1U, sizeof(*i));
    assert(i != NULL);
    assert(umi_repository_gitmodules_parse("[submodule \"x\"]\n url = x\n", i) == UMI_STATUS_PARSE_ERROR);
    free(i);
    return 0;
}
