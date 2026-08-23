/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_runtime/validation_profiles/gradle.c
 *
 * PURPOSE:
 *   Implement the reusable gradle validation profile.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_runtime/validation_profiles/gradle.h"
#include "support.h"

UmiStatus umi_ai_coding_validation_profile_gradle(
    const char *workspace_root,
    UmiAiCodingValidationPlan *out_plan)
{
    UmiStatus status;
    if (workspace_root == NULL || out_plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_ai_coding_validation_plan_init(out_plan);
    status = umi_ai_coding_validation_profile_add(
        out_plan, "gradle.test", UMI_AI_CODING_VALIDATION_TEST,
        "Gradle Tests", "gradle", "test",
        workspace_root, 1, 300000U);
    if (status != UMI_STATUS_OK) return status;
    status = umi_ai_coding_validation_profile_add(
        out_plan, "gradle.build", UMI_AI_CODING_VALIDATION_BUILD,
        "Gradle Build", "gradle", "build",
        workspace_root, 1, 300000U);
    if (status != UMI_STATUS_OK) return status;
    return UMI_STATUS_OK;
}
