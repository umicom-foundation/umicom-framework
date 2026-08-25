/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/runtime_bootstrap/test_resolution_trace.c
 *
 * PURPOSE:
 *   Focused regression coverage for Record bounded, human-readable dependency-resolution decisions.
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>
#include "umicom/runtime/bootstrap/resolution_trace.h"

#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {

    UmiBootstrapResolutionTrace t; umi_bootstrap_resolution_trace_init(&t);
    CHECK(umi_bootstrap_resolution_trace_append(&t,"resolve:svc.a")==UMI_STATUS_OK);
    CHECK(t.count==1U && strcmp(t.steps[0],"resolve:svc.a")==0);
    return 0;
}
