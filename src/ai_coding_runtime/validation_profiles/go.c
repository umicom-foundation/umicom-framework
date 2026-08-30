/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_runtime/validation_profiles/go.c
 *
 * PURPOSE:
 *   Implement the reusable go validation profile.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_runtime/validation_profiles/go.h"
#include "support.h"

UmiStatus umi_ai_coding_validation_profile_go(
    const char *workspace_root,
    UmiAiCodingValidationPlan *out_plan)
{
    UmiStatus status;
    if (workspace_root == NULL || out_plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_ai_coding_validation_plan_init(out_plan);
    status = umi_ai_coding_validation_profile_add(
        out_plan, "go.test", UMI_AI_CODING_VALIDATION_TEST,
        "Go Tests", "go", "test ./...",
        workspace_root, 1, 300000U);
    if (status != UMI_STATUS_OK) return status;
    status = umi_ai_coding_validation_profile_add(
        out_plan, "go.build", UMI_AI_CODING_VALIDATION_BUILD,
        "Go Build", "go", "build ./...",
        workspace_root, 1, 180000U);
    if (status != UMI_STATUS_OK) return status;
    return UMI_STATUS_OK;
}
