/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_runtime/validation_profiles/meson.c
 *
 * PURPOSE:
 *   Implement the reusable meson validation profile.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_runtime/validation_profiles/meson.h"
#include "support.h"

UmiStatus umi_ai_coding_validation_profile_meson(
    const char *workspace_root,
    UmiAiCodingValidationPlan *out_plan)
{
    UmiStatus status;
    if (workspace_root == NULL || out_plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_ai_coding_validation_plan_init(out_plan);
    status = umi_ai_coding_validation_profile_add(
        out_plan, "meson.setup", UMI_AI_CODING_VALIDATION_CONFIGURE,
        "Meson Setup", "meson", "setup build/ai-validation",
        workspace_root, 1, 120000U);
    if (status != UMI_STATUS_OK) return status;
    status = umi_ai_coding_validation_profile_add(
        out_plan, "meson.compile", UMI_AI_CODING_VALIDATION_BUILD,
        "Meson Compile", "meson", "compile -C build/ai-validation",
        workspace_root, 1, 180000U);
    if (status != UMI_STATUS_OK) return status;
    status = umi_ai_coding_validation_profile_add(
        out_plan, "meson.test", UMI_AI_CODING_VALIDATION_TEST,
        "Meson Tests", "meson", "test -C build/ai-validation --print-errorlogs",
        workspace_root, 1, 300000U);
    if (status != UMI_STATUS_OK) return status;
    return UMI_STATUS_OK;
}
