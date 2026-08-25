/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/runtime_bootstrap/test_factory_descriptor.c
 *
 * PURPOSE:
 *   Focused regression coverage for Describe named factories that create services for the canonical service registry.
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>
#include "umicom/runtime/bootstrap/service_key.h"
#include "umicom/runtime/bootstrap/factory_descriptor.h"

#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {

    UmiBootstrapServiceKey key; UmiBootstrapFactoryDescriptor d;
    CHECK(umi_bootstrap_service_key_init(&key,"svc.alpha","")==UMI_STATUS_OK);
    CHECK(umi_bootstrap_factory_descriptor_init(&d,"factory.alpha",&key,10)==UMI_STATUS_OK);
    CHECK(umi_bootstrap_factory_descriptor_valid(&d));
    return 0;
}
