/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_runtime/validation_profiles/npm.c
 *
 * PURPOSE:
 *   Implement the reusable npm validation profile.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_runtime/validation_profiles/npm.h"
#include "support.h"

UmiStatus umi_ai_coding_validation_profile_npm(
    const char *workspace_root,
    UmiAiCodingValidationPlan *out_plan)
{
    UmiStatus status;
    if (workspace_root == NULL || out_plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_ai_coding_validation_plan_init(out_plan);
    status = umi_ai_coding_validation_profile_add(
        out_plan, "npm.test", UMI_AI_CODING_VALIDATION_TEST,
        "NPM Tests", "npm", "test -- --runInBand",
        workspace_root, 0, 300000U);
    if (status != UMI_STATUS_OK) return status;
    status = umi_ai_coding_validation_profile_add(
        out_plan, "npm.build", UMI_AI_CODING_VALIDATION_BUILD,
        "NPM Build", "npm", "run build",
        workspace_root, 1, 300000U);
    if (status != UMI_STATUS_OK) return status;
    return UMI_STATUS_OK;
}
