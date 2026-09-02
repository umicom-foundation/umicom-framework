/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_runtime/validation_profiles/maven.c
 *
 * PURPOSE:
 *   Implement the reusable maven validation profile.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_runtime/validation_profiles/maven.h"
#include "support.h"

/*
 * Provide the ai coding validation profile maven operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_coding_validation_profile_maven(
    const char *workspace_root,
    UmiAiCodingValidationPlan *out_plan)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace_root == NULL || out_plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_ai_coding_validation_plan_init(out_plan);
    status = umi_ai_coding_validation_profile_add(
        out_plan, "maven.test", UMI_AI_CODING_VALIDATION_TEST,
        "Maven Tests", "mvn", "test",
        workspace_root, 1, 300000U);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_ai_coding_validation_profile_add(
        out_plan, "maven.package", UMI_AI_CODING_VALIDATION_PACKAGE,
        "Maven Package", "mvn", "package -DskipTests",
        workspace_root, 0, 300000U);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    return UMI_STATUS_OK;
}
