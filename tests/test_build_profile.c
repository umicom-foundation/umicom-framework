/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_build_profile.c
 *
 * PURPOSE:
 *   Verify build-profile defaults, validation and equality.
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
#include "umicom/build/profile.h"

int main(void)
{
    UmiBuildProfile a;
    UmiBuildProfile b;
    char message[256];
    umi_build_profile_init(&a);
    assert(umi_build_profile_set(&a, "debug", ".", "build/debug") == UMI_STATUS_OK);
    assert(umi_build_profile_validate(&a, message, sizeof(message)) == UMI_STATUS_OK);
    b = a;
    assert(umi_build_profile_equal(&a, &b));
    b.parallel_jobs += 1U;
    assert(!umi_build_profile_equal(&a, &b));
    return 0;
}
