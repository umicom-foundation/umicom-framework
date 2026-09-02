/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/runtime_bootstrap/test_binding_registry.c
 *
 * PURPOSE:
 *   Implement the test binding registry behavior for
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
 * File: tests/runtime_bootstrap/test_binding_registry.c
 *
 * PURPOSE:
 *   Focused regression coverage for Maintain deterministic binding candidates and select the highest-priority match.
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>
#include "umicom/runtime/bootstrap/service_key.h"
#include "umicom/runtime/bootstrap/binding_descriptor.h"
#include "umicom/runtime/bootstrap/binding_registry.h"

#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {

    UmiBootstrapServiceKey key;
    UmiBootstrapBindingDescriptor a, b;
    UmiBootstrapBindingRegistry r;
    CHECK(umi_bootstrap_service_key_init(&key, "svc.data", "") == UMI_STATUS_OK);
    CHECK(umi_bootstrap_binding_descriptor_init(&a, &key, UMI_BOOTSTRAP_BIND_FACTORY, "factory.a", "", 10) == UMI_STATUS_OK);
    CHECK(umi_bootstrap_binding_descriptor_init(&b, &key, UMI_BOOTSTRAP_BIND_FACTORY, "factory.b", "", 20) == UMI_STATUS_OK);
    umi_bootstrap_binding_registry_init(&r);
    CHECK(umi_bootstrap_binding_registry_add(&r, &a) == UMI_STATUS_OK);
    CHECK(umi_bootstrap_binding_registry_add(&r, &b) == UMI_STATUS_OK);
    CHECK(strcmp(umi_bootstrap_binding_registry_find_best(&r, &key)->implementation_id, "factory.b") == 0);
    return 0;
}
