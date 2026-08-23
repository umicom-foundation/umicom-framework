/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_runtime/validation_profiles/cargo.c
 *
 * PURPOSE:
 *   Implement the reusable cargo validation profile.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_runtime/validation_profiles/cargo.h"
#include "support.h"

UmiStatus umi_ai_coding_validation_profile_cargo(
    const char *workspace_root,
    UmiAiCodingValidationPlan *out_plan)
{
    UmiStatus status;
    if (workspace_root == NULL || out_plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_ai_coding_validation_plan_init(out_plan);
    status = umi_ai_coding_validation_profile_add(
        out_plan, "cargo.check", UMI_AI_CODING_VALIDATION_BUILD,
        "Cargo Check", "cargo", "check",
        workspace_root, 1, 180000U);
    if (status != UMI_STATUS_OK) return status;
    status = umi_ai_coding_validation_profile_add(
        out_plan, "cargo.test", UMI_AI_CODING_VALIDATION_TEST,
        "Cargo Tests", "cargo", "test",
        workspace_root, 1, 300000U);
    if (status != UMI_STATUS_OK) return status;
    return UMI_STATUS_OK;
}
