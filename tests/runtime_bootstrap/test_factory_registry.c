/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/runtime_bootstrap/test_factory_registry.c
 *
 * PURPOSE:
 *   Implement the test factory registry behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/runtime_bootstrap/test_factory_registry.c
 *
 * PURPOSE:
 *   Focused regression coverage for Maintain and query bounded Framework service-factory definitions.
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>
#include "umicom/runtime/bootstrap/service_key.h"
#include "umicom/runtime/bootstrap/factory_descriptor.h"
#include "umicom/runtime/bootstrap/factory_registry.h"

#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {

    UmiBootstrapServiceKey key; UmiBootstrapFactoryDescriptor d; UmiBootstrapFactoryRegistry r;
    CHECK(umi_bootstrap_service_key_init(&key,"svc.alpha","")==UMI_STATUS_OK);
    CHECK(umi_bootstrap_factory_descriptor_init(&d,"factory.alpha",&key,10)==UMI_STATUS_OK);
    umi_bootstrap_factory_registry_init(&r);
    CHECK(umi_bootstrap_factory_registry_add(&r,&d)==UMI_STATUS_OK);
    CHECK(umi_bootstrap_factory_registry_find(&r,"factory.alpha")!=NULL);
    return 0;
}
