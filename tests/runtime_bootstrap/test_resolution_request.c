/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/runtime_bootstrap/test_resolution_request.c
 *
 * PURPOSE:
 *   Implement the test resolution request behavior for
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
 * File: tests/runtime_bootstrap/test_resolution_request.c
 *
 * PURPOSE:
 *   Focused regression coverage for Describe one dependency-resolution request including scope and optionality.
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>
#include "umicom/runtime/bootstrap/service_key.h"
#include "umicom/runtime/bootstrap/resolution_request.h"

#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {

    UmiBootstrapServiceKey key; UmiBootstrapResolutionRequest r;
    CHECK(umi_bootstrap_service_key_init(&key,"svc.a","")==UMI_STATUS_OK);
    CHECK(umi_bootstrap_resolution_request_init(&r,&key,UMI_BOOTSTRAP_SCOPE_APPLICATION,true)==UMI_STATUS_OK);
    CHECK(r.required);
    return 0;
}
