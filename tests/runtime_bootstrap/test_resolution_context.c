/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/runtime_bootstrap/test_resolution_context.c
 *
 * PURPOSE:
 *   Implement the test resolution context behavior for
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
 * File: tests/runtime_bootstrap/test_resolution_context.c
 *
 * PURPOSE:
 *   Focused regression coverage for Detect recursive resolution cycles using an explicit dependency stack.
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>
#include "umicom/runtime/bootstrap/service_key.h"
#include "umicom/runtime/bootstrap/resolution_context.h"

#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {

    UmiBootstrapServiceKey key; UmiBootstrapResolutionContext c;
    CHECK(umi_bootstrap_service_key_init(&key,"svc.a","")==UMI_STATUS_OK);
    umi_bootstrap_resolution_context_init(&c);
    CHECK(umi_bootstrap_resolution_context_push(&c,&key)==UMI_STATUS_OK);
    CHECK(umi_bootstrap_resolution_context_contains(&c,&key));
    CHECK(umi_bootstrap_resolution_context_push(&c,&key)==UMI_STATUS_INVALID_STATE);
    CHECK(umi_bootstrap_resolution_context_pop(&c)==UMI_STATUS_OK);
    return 0;
}
