/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_runtime/validation_profiles/maven.c
 *
 * PURPOSE:
 *   Implement the reusable maven validation profile.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_runtime/validation_profiles/maven.h"
#include "support.h"

UmiStatus umi_ai_coding_validation_profile_maven(
    const char *workspace_root,
    UmiAiCodingValidationPlan *out_plan)
{
    UmiStatus status;
    if (workspace_root == NULL || out_plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_ai_coding_validation_plan_init(out_plan);
    status = umi_ai_coding_validation_profile_add(
        out_plan, "maven.test", UMI_AI_CODING_VALIDATION_TEST,
        "Maven Tests", "mvn", "test",
        workspace_root, 1, 300000U);
    if (status != UMI_STATUS_OK) return status;
    status = umi_ai_coding_validation_profile_add(
        out_plan, "maven.package", UMI_AI_CODING_VALIDATION_PACKAGE,
        "Maven Package", "mvn", "package -DskipTests",
        workspace_root, 0, 300000U);
    if (status != UMI_STATUS_OK) return status;
    return UMI_STATUS_OK;
}
