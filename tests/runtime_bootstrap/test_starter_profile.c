/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/runtime_bootstrap/test_starter_profile.c
 *
 * PURPOSE:
 *   Focused regression coverage for Group application starters into reusable environment/application profiles.
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>
#include "umicom/runtime/bootstrap/starter_descriptor.h"
#include "umicom/runtime/bootstrap/starter_catalogue.h"
#include "umicom/runtime/bootstrap/starter_profile.h"

#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {

    UmiBootstrapStarterDescriptor s; UmiBootstrapStarterCatalogue c; UmiBootstrapStarterProfile p;
    umi_bootstrap_starter_catalogue_init(&c);
    CHECK(umi_bootstrap_starter_descriptor_init(&s,"starter.web","feature.web",5)==UMI_STATUS_OK);
    CHECK(umi_bootstrap_starter_catalogue_add(&c,&s)==UMI_STATUS_OK);
    CHECK(umi_bootstrap_starter_profile_init(&p,"profile.dev")==UMI_STATUS_OK);
    CHECK(umi_bootstrap_starter_profile_add(&p,&c,"starter.web")==UMI_STATUS_OK);
    CHECK(p.starters.count==1U);
    return 0;
}
