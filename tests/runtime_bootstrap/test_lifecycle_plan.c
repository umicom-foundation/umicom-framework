/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/runtime_bootstrap/test_lifecycle_plan.c
 *
 * PURPOSE:
 *   Focused regression coverage for Collect and sort lifecycle hooks deterministically by phase and order.
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>
#include "umicom/runtime/bootstrap/lifecycle_hook.h"
#include "umicom/runtime/bootstrap/lifecycle_plan.h"

#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {

    UmiBootstrapLifecyclePlan p; UmiBootstrapLifecycleHook a,b;
    umi_bootstrap_lifecycle_plan_init(&p);
    CHECK(umi_bootstrap_lifecycle_hook_init(&a,"hook.ready","svc.a",UMI_BOOTSTRAP_PHASE_READY,0)==UMI_STATUS_OK);
    CHECK(umi_bootstrap_lifecycle_hook_init(&b,"hook.start","svc.a",UMI_BOOTSTRAP_PHASE_START,0)==UMI_STATUS_OK);
    CHECK(umi_bootstrap_lifecycle_plan_add(&p,&a)==UMI_STATUS_OK);
    CHECK(umi_bootstrap_lifecycle_plan_add(&p,&b)==UMI_STATUS_OK);
    CHECK(strcmp(p.hooks[0].hook_id,"hook.start")==0);
    return 0;
}
