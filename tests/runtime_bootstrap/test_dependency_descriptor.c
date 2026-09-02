/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/runtime_bootstrap/test_dependency_descriptor.c
 *
 * PURPOSE:
 *   Implement the test dependency descriptor behavior for
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
 * File: tests/runtime_bootstrap/test_dependency_descriptor.c
 *
 * PURPOSE:
 *   Focused regression coverage for Declare required, optional and collection dependencies between Framework services.
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>
#include "umicom/runtime/bootstrap/dependency_descriptor.h"

#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {

    UmiBootstrapDependencyDescriptor d;
    CHECK(umi_bootstrap_dependency_descriptor_init(&d, "svc.data", "", true, false) == UMI_STATUS_OK);
    CHECK(d.required && !d.collection);
    return 0;
}
