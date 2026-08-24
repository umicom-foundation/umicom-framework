/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_control/test_control_types_boundary.c
 *
 * PURPOSE:
 *   Regression coverage for repository control types boundary semantics.
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
#include <string.h>
#include "umicom/repository/control_types.h"

int main(void)
{
    assert(UMI_REPOSITORY_CONTROL_ITEM_CAPACITY >= 64U);
    assert(UMI_REPOSITORY_CONTROL_SHA_CAPACITY == 65U);
    return 0;
}
