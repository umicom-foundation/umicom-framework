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
#include "../value_archive/construction_cases.h"
#include <string.h>
#include "umicom/runtime/bootstrap/bootstrap_stage.h"
#include "umicom/runtime/bootstrap/bootstrap_plan.h"

#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */

/* A rejected replacement must preserve the previously accepted bootstrap stage value. */
static int UmiBootstrapStageConstructionCases(void)
{
    UmiBootstrapStage *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_bootstrap_stage_init(value, "bootstrap.start", 20) == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->stage_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_bootstrap_stage_init(value, oversized, 20) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_bootstrap_stage_init(value, value->stage_id, 20) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->stage_id, "bootstrap.start") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void) {
    if (UmiBootstrapStageConstructionCases() != 0) return 1;


    UmiBootstrapStage a,b; UmiBootstrapPlan p; umi_bootstrap_plan_init(&p);
    CHECK(umi_bootstrap_stage_init(&a,"bootstrap.start",20)==UMI_STATUS_OK);
    CHECK(umi_bootstrap_stage_init(&b,"bootstrap.configure",10)==UMI_STATUS_OK);
    CHECK(umi_bootstrap_plan_add_stage(&p,&a)==UMI_STATUS_OK);
    CHECK(umi_bootstrap_plan_add_stage(&p,&b)==UMI_STATUS_OK);
    CHECK(strcmp(p.stages[0].stage_id,"bootstrap.configure")==0);
    return 0;
}
