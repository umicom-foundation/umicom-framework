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

int main(void) {

    UmiBootstrapDependencyDescriptor d;
    CHECK(umi_bootstrap_dependency_descriptor_init(&d, "svc.data", "", true, false) == UMI_STATUS_OK);
    CHECK(d.required && !d.collection);
    return 0;
}
