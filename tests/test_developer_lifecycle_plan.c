/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_developer_lifecycle_plan.c
 *
 * PURPOSE:
 *   Verify Umicom development plans place checks before compilation and keep
 *   optional vulnerability tooling visible instead of claiming it ran.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/lifecycle_plan.h"

#include <stdlib.h>

int main(void)
{
    UmiDeveloperLifecyclePlanRequest request;
    UmiDeveloperLifecyclePlan plan;

    umi_developer_lifecycle_plan_request_init(
        &request, UMI_DEVELOPER_LIFECYCLE_COMPLETE);
    if (umi_developer_lifecycle_plan_build(&request, &plan) != UMI_STATUS_OK ||
        plan.stage_count < 10U ||
        plan.stages[0].stage != UMI_DEVELOPER_STAGE_ENVIRONMENT ||
        plan.stages[6].stage !=
            UMI_DEVELOPER_STAGE_DEPENDENCY_VULNERABILITIES ||
        plan.stages[6].required ||
        plan.stages[7].stage != UMI_DEVELOPER_STAGE_CONFIGURE ||
        plan.stages[plan.stage_count - 1U].stage !=
            UMI_DEVELOPER_STAGE_REPOSITORY_PUSH) {
        return EXIT_FAILURE;
    }

    request.require_external_vulnerability_scanner = 1;
    if (umi_developer_lifecycle_plan_build(&request, &plan) != UMI_STATUS_OK ||
        !plan.stages[6].required) {
        return EXIT_FAILURE;
    }

    umi_developer_lifecycle_plan_request_init(
        &request, UMI_DEVELOPER_LIFECYCLE_UPDATE);
    if (umi_developer_lifecycle_plan_build(&request, &plan) != UMI_STATUS_OK ||
        plan.stage_count != 2U ||
        plan.stages[1].stage != UMI_DEVELOPER_STAGE_REPOSITORY_UPDATE ||
        !plan.stages[1].uses_network) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
