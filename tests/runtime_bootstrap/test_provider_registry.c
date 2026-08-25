/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/runtime_bootstrap/test_provider_registry.c
 *
 * PURPOSE:
 *   Focused regression coverage for Register and rank service-provider modules deterministically.
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>
#include "umicom/runtime/bootstrap/provider_descriptor.h"
#include "umicom/runtime/bootstrap/provider_registry.h"

#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {

    UmiBootstrapProviderDescriptor a,b; UmiBootstrapProviderRegistry r;
    CHECK(umi_bootstrap_provider_descriptor_init(&a,"provider.a","module.a",5)==UMI_STATUS_OK);
    CHECK(umi_bootstrap_provider_descriptor_init(&b,"provider.b","module.b",9)==UMI_STATUS_OK);
    umi_bootstrap_provider_registry_init(&r);
    CHECK(umi_bootstrap_provider_registry_add(&r,&a)==UMI_STATUS_OK);
    CHECK(umi_bootstrap_provider_registry_add(&r,&b)==UMI_STATUS_OK);
    CHECK(strcmp(umi_bootstrap_provider_registry_best(&r)->provider_id,"provider.b")==0);
    return 0;
}
