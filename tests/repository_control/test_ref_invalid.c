/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_control/test_ref_invalid.c
 *
 * PURPOSE:
 *   Regression coverage for repository ref invalid semantics.
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
#include "umicom/repository/ref.h"

int main(void)
{
    assert(!umi_repository_ref_is_valid("../main"));
    assert(!umi_repository_ref_is_valid("bad ref"));
    assert(!umi_repository_ref_is_valid("topic..name"));
    return 0;
}
