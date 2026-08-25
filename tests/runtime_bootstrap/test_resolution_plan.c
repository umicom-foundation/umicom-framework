/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/runtime_bootstrap/test_resolution_plan.c
 *
 * PURPOSE:
 *   Focused regression coverage for Build deterministic ordered service-resolution plans.
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>
#include "umicom/runtime/bootstrap/service_key.h"
#include "umicom/runtime/bootstrap/resolution_plan.h"

#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {

    UmiBootstrapServiceKey key; UmiBootstrapResolutionPlan p;
    CHECK(umi_bootstrap_service_key_init(&key,"svc.a","")==UMI_STATUS_OK);
    umi_bootstrap_resolution_plan_init(&p);
    CHECK(umi_bootstrap_resolution_plan_add(&p,&key)==UMI_STATUS_OK);
    CHECK(umi_bootstrap_resolution_plan_add(&p,&key)==UMI_STATUS_ALREADY_EXISTS);
    return 0;
}
