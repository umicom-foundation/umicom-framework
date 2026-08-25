/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/runtime_bootstrap/test_binding_descriptor.c
 *
 * PURPOSE:
 *   Focused regression coverage for Describe factory, instance and alias bindings selected by dependency resolution.
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>
#include "umicom/runtime/bootstrap/service_key.h"
#include "umicom/runtime/bootstrap/binding_descriptor.h"

#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {

    UmiBootstrapServiceKey key;
    UmiBootstrapBindingDescriptor b;
    CHECK(umi_bootstrap_service_key_init(&key, "svc.data", "") == UMI_STATUS_OK);
    CHECK(umi_bootstrap_binding_descriptor_init(&b, &key, UMI_BOOTSTRAP_BIND_FACTORY,
        "factory.data", "", 20) == UMI_STATUS_OK);
    CHECK(umi_bootstrap_binding_descriptor_valid(&b));
    return 0;
}
