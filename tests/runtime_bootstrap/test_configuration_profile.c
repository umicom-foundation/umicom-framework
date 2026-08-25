/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/runtime_bootstrap/test_configuration_profile.c
 *
 * PURPOSE:
 *   Focused regression coverage for Represent named runtime profiles and parent-profile relationships.
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>
#include "umicom/runtime/bootstrap/configuration_profile.h"

#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {

    UmiBootstrapConfigurationProfile p;
    CHECK(umi_bootstrap_configuration_profile_init(&p,"prod","base",5,true)==UMI_STATUS_OK);
    CHECK(umi_bootstrap_configuration_profile_matches(&p,"prod"));
    return 0;
}
