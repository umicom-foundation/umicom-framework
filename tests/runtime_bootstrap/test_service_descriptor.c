/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/runtime_bootstrap/test_service_descriptor.c
 *
 * PURPOSE:
 *   Implement the test service descriptor behavior for
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
 * File: tests/runtime_bootstrap/test_service_descriptor.c
 *
 * PURPOSE:
 *   Focused regression coverage for Describe Framework services, ownership scope and lifetime without global variables.
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>
#include "umicom/runtime/bootstrap/service_descriptor.h"

#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {

    UmiBootstrapServiceDescriptor d;
    CHECK(umi_bootstrap_service_descriptor_init(&d, "svc.alpha", "", "provider.core",
        UMI_BOOTSTRAP_SCOPE_APPLICATION, UMI_BOOTSTRAP_LIFETIME_EAGER, 10) == UMI_STATUS_OK);
    CHECK(umi_bootstrap_service_descriptor_valid(&d));
    CHECK(d.enabled);
    return 0;
}
