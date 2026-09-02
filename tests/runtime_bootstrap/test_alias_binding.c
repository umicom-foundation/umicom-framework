/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/runtime_bootstrap/test_alias_binding.c
 *
 * PURPOSE:
 *   Implement the test alias binding behavior for
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
 * File: tests/runtime_bootstrap/test_alias_binding.c
 *
 * PURPOSE:
 *   Focused regression coverage for Map alternate service identifiers without copying or owning instances.
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>
#include "umicom/runtime/bootstrap/service_key.h"
#include "umicom/runtime/bootstrap/binding_descriptor.h"
#include "umicom/runtime/bootstrap/alias_binding.h"

#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {

    UmiBootstrapServiceKey key; UmiBootstrapBindingDescriptor b;
    CHECK(umi_bootstrap_service_key_init(&key,"svc.alias","")==UMI_STATUS_OK);
    CHECK(umi_bootstrap_binding_descriptor_init(&b,&key,UMI_BOOTSTRAP_BIND_FACTORY,"factory.alias","",0)==UMI_STATUS_OK);
    CHECK(umi_bootstrap_alias_binding_configure(&b,"svc.target")==UMI_STATUS_OK);
    CHECK(umi_bootstrap_alias_binding_valid(&b));
    return 0;
}
