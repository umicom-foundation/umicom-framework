/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_control/test_head_set_contract.c
 *
 * PURPOSE:
 *   Regression coverage for repository head set contract semantics.
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
#include "umicom/repository/head_set.h"

int main(void)
{
    UmiRepositoryHeadSet *s = calloc(1U, sizeof(*s));
    assert(s != NULL); umi_repository_head_set_init(s);
    assert(umi_repository_head_set_upsert(s, "framework", "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa") == UMI_STATUS_OK);
    assert(umi_repository_head_set_find(s, "framework") != NULL);
    free(s);
    return 0;
}
