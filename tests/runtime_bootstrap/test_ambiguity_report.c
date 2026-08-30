/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/runtime_bootstrap/test_ambiguity_report.c
 *
 * PURPOSE:
 *   Implement the test ambiguity report behavior for
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
 * File: tests/runtime_bootstrap/test_ambiguity_report.c
 *
 * PURPOSE:
 *   Focused regression coverage for Detect equally-ranked bindings that make a service resolution ambiguous.
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>
#include "umicom/runtime/bootstrap/service_key.h"
#include "umicom/runtime/bootstrap/binding_descriptor.h"
#include "umicom/runtime/bootstrap/binding_registry.h"
#include "umicom/runtime/bootstrap/ambiguity_report.h"

#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {

    UmiBootstrapServiceKey key; UmiBootstrapBindingDescriptor a,b; UmiBootstrapBindingRegistry reg;
    CHECK(umi_bootstrap_service_key_init(&key,"svc.a","")==UMI_STATUS_OK);
    CHECK(umi_bootstrap_binding_descriptor_init(&a,&key,UMI_BOOTSTRAP_BIND_FACTORY,"factory.a","",5)==UMI_STATUS_OK);
    CHECK(umi_bootstrap_binding_descriptor_init(&b,&key,UMI_BOOTSTRAP_BIND_FACTORY,"factory.b","",5)==UMI_STATUS_OK);
    umi_bootstrap_binding_registry_init(&reg); CHECK(umi_bootstrap_binding_registry_add(&reg,&a)==UMI_STATUS_OK);
    CHECK(umi_bootstrap_binding_registry_add(&reg,&b)==UMI_STATUS_OK);
    CHECK(umi_bootstrap_binding_ambiguity_count(&reg,&key)==2U);
    return 0;
}
