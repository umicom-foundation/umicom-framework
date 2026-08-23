/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_runtime/validation_profiles/python.c
 *
 * PURPOSE:
 *   Implement the reusable python validation profile.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_runtime/validation_profiles/python.h"
#include "support.h"

UmiStatus umi_ai_coding_validation_profile_python(
    const char *workspace_root,
    UmiAiCodingValidationPlan *out_plan)
{
    UmiStatus status;
    if (workspace_root == NULL || out_plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_ai_coding_validation_plan_init(out_plan);
    status = umi_ai_coding_validation_profile_add(
        out_plan, "python.compile", UMI_AI_CODING_VALIDATION_BUILD,
        "Python Compile", "python", "-m compileall -q .",
        workspace_root, 1, 180000U);
    if (status != UMI_STATUS_OK) return status;
    status = umi_ai_coding_validation_profile_add(
        out_plan, "python.test", UMI_AI_CODING_VALIDATION_TEST,
        "Pytest", "python", "-m pytest -q",
        workspace_root, 0, 300000U);
    if (status != UMI_STATUS_OK) return status;
    return UMI_STATUS_OK;
}
