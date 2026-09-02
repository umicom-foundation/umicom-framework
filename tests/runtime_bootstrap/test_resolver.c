/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/runtime_bootstrap/test_resolver.c
 *
 * PURPOSE:
 *   Implement the test resolver behavior for
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
 * File: tests/runtime_bootstrap/test_resolver.c
 *
 * PURPOSE:
 *   Focused regression coverage for Select the best enabled binding using qualifier, primary flag and priority.
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>
#include "umicom/runtime/bootstrap/service_key.h"
#include "umicom/runtime/bootstrap/binding_descriptor.h"
#include "umicom/runtime/bootstrap/binding_registry.h"
#include "umicom/runtime/bootstrap/resolution_request.h"
#include "umicom/runtime/bootstrap/resolution_trace.h"
#include "umicom/runtime/bootstrap/resolver.h"

#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {

    UmiBootstrapServiceKey key; UmiBootstrapBindingDescriptor b; UmiBootstrapBindingRegistry reg;
    UmiBootstrapResolutionRequest req; UmiBootstrapResolutionTrace trace; const UmiBootstrapBindingDescriptor *out=NULL;
    CHECK(umi_bootstrap_service_key_init(&key,"svc.a","")==UMI_STATUS_OK);
    CHECK(umi_bootstrap_binding_descriptor_init(&b,&key,UMI_BOOTSTRAP_BIND_FACTORY,"factory.a","",7)==UMI_STATUS_OK);
    umi_bootstrap_binding_registry_init(&reg); CHECK(umi_bootstrap_binding_registry_add(&reg,&b)==UMI_STATUS_OK);
    CHECK(umi_bootstrap_resolution_request_init(&req,&key,UMI_BOOTSTRAP_SCOPE_APPLICATION,true)==UMI_STATUS_OK);
    umi_bootstrap_resolution_trace_init(&trace);
    CHECK(umi_bootstrap_resolver_select(&reg,&req,&out,&trace)==UMI_STATUS_OK);
    CHECK(out!=NULL && trace.count==1U);
    return 0;
}
