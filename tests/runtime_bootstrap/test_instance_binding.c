/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/runtime_bootstrap/test_instance_binding.c
 *
 * PURPOSE:
 *   Implement the test instance binding behavior for
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
 * File: tests/runtime_bootstrap/test_instance_binding.c
 *
 * PURPOSE:
 *   Focused regression coverage for Validate direct instance bindings for pre-built Framework services.
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>
#include "umicom/runtime/bootstrap/service_key.h"
#include "umicom/runtime/bootstrap/binding_descriptor.h"
#include "umicom/runtime/bootstrap/instance_binding.h"

#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {

    UmiBootstrapServiceKey key; UmiBootstrapBindingDescriptor b; int value=42;
    CHECK(umi_bootstrap_service_key_init(&key,"svc.alpha","")==UMI_STATUS_OK);
    CHECK(umi_bootstrap_binding_descriptor_init(&b,&key,UMI_BOOTSTRAP_BIND_INSTANCE,"placeholder","",0)==UMI_STATUS_OK);
    CHECK(umi_bootstrap_instance_binding_configure(&b,&value)==UMI_STATUS_OK);
    CHECK(umi_bootstrap_instance_binding_ready(&b));
    return 0;
}
