/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/runtime_bootstrap/test_bootstrap_plan.c
 *
 * PURPOSE:
 *   Implement the test bootstrap plan behavior for
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
 * File: tests/runtime_bootstrap/test_bootstrap_plan.c
 *
 * PURPOSE:
 *   Focused regression coverage for Build ordered application bootstrap stages.
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>
#include "umicom/runtime/bootstrap/bootstrap_stage.h"
#include "umicom/runtime/bootstrap/bootstrap_plan.h"

#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {

    UmiBootstrapStage a,b; UmiBootstrapPlan p; umi_bootstrap_plan_init(&p);
    CHECK(umi_bootstrap_stage_init(&a,"bootstrap.start",20)==UMI_STATUS_OK);
    CHECK(umi_bootstrap_stage_init(&b,"bootstrap.configure",10)==UMI_STATUS_OK);
    CHECK(umi_bootstrap_plan_add_stage(&p,&a)==UMI_STATUS_OK);
    CHECK(umi_bootstrap_plan_add_stage(&p,&b)==UMI_STATUS_OK);
    CHECK(strcmp(p.stages[0].stage_id,"bootstrap.configure")==0);
    return 0;
}
