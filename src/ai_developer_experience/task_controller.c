/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_developer_experience/task_controller.c
 *
 * PURPOSE:
 *   Implement high-level task controls while preserving coding runtime authority.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_developer_experience/task_controller.h"

/*
 * Provide the ai developer task cancel operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_developer_task_cancel(
    UmiAiDeveloperExperiencePlatform *platform)
{
    UmiAiCodingRuntimePlatform *coding;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    coding = umi_ai_developer_experience_platform_coding(platform);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (coding == NULL) return UMI_STATUS_INVALID_STATE;

    umi_ai_coding_runtime_platform_cancel(coding);
    return umi_ai_developer_experience_platform_refresh(platform);
}

/*
 * Provide the ai developer task continue repair operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_developer_task_continue_repair(
    UmiAiDeveloperExperiencePlatform *platform)
{
    UmiAiCodingRuntimePlatform *coding;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    coding = umi_ai_developer_experience_platform_coding(platform);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (coding == NULL) return UMI_STATUS_INVALID_STATE;

    status = umi_ai_coding_runtime_platform_continue_repair(coding);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    return umi_ai_developer_experience_platform_refresh(platform);
}

/*
 * Provide the ai developer task apply pending operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_developer_task_apply_pending(
    UmiAiDeveloperExperiencePlatform *platform)
{
    UmiAiCodingRuntimePlatform *coding;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /* Apply this operation only while the related capability or state is available. */
    if (!umi_ai_developer_patch_review_service_ready_to_approve(
            umi_ai_developer_experience_platform_review(platform))) {
        return UMI_STATUS_INVALID_STATE;
    }

    coding = umi_ai_developer_experience_platform_coding(platform);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (coding == NULL) return UMI_STATUS_INVALID_STATE;

    status = umi_ai_coding_runtime_platform_apply(coding);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    return umi_ai_developer_experience_platform_refresh(platform);
}

/*
 * Provide the ai developer task reject pending operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_developer_task_reject_pending(
    UmiAiDeveloperExperiencePlatform *platform)
{
    UmiAiCodingRuntimePlatform *coding;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    coding = umi_ai_developer_experience_platform_coding(platform);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (coding == NULL) return UMI_STATUS_INVALID_STATE;

    status = umi_ai_coding_runtime_platform_reject(coding);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    return umi_ai_developer_experience_platform_refresh(platform);
}
