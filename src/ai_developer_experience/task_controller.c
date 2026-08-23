/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_developer_experience/task_controller.c
 *
 * PURPOSE:
 *   Implement high-level task controls while preserving coding runtime authority.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_developer_experience/task_controller.h"

UmiStatus umi_ai_developer_task_cancel(
    UmiAiDeveloperExperiencePlatform *platform)
{
    UmiAiCodingRuntimePlatform *coding;

    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    coding = umi_ai_developer_experience_platform_coding(platform);
    if (coding == NULL) return UMI_STATUS_INVALID_STATE;

    umi_ai_coding_runtime_platform_cancel(coding);
    return umi_ai_developer_experience_platform_refresh(platform);
}

UmiStatus umi_ai_developer_task_continue_repair(
    UmiAiDeveloperExperiencePlatform *platform)
{
    UmiAiCodingRuntimePlatform *coding;
    UmiStatus status;

    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    coding = umi_ai_developer_experience_platform_coding(platform);
    if (coding == NULL) return UMI_STATUS_INVALID_STATE;

    status = umi_ai_coding_runtime_platform_continue_repair(coding);
    if (status != UMI_STATUS_OK) return status;

    return umi_ai_developer_experience_platform_refresh(platform);
}

UmiStatus umi_ai_developer_task_apply_pending(
    UmiAiDeveloperExperiencePlatform *platform)
{
    UmiAiCodingRuntimePlatform *coding;
    UmiStatus status;

    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    if (!umi_ai_developer_patch_review_service_ready_to_approve(
            umi_ai_developer_experience_platform_review(platform))) {
        return UMI_STATUS_INVALID_STATE;
    }

    coding = umi_ai_developer_experience_platform_coding(platform);
    if (coding == NULL) return UMI_STATUS_INVALID_STATE;

    status = umi_ai_coding_runtime_platform_apply(coding);
    if (status != UMI_STATUS_OK) return status;

    return umi_ai_developer_experience_platform_refresh(platform);
}

UmiStatus umi_ai_developer_task_reject_pending(
    UmiAiDeveloperExperiencePlatform *platform)
{
    UmiAiCodingRuntimePlatform *coding;
    UmiStatus status;

    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    coding = umi_ai_developer_experience_platform_coding(platform);
    if (coding == NULL) return UMI_STATUS_INVALID_STATE;

    status = umi_ai_coding_runtime_platform_reject(coding);
    if (status != UMI_STATUS_OK) return status;

    return umi_ai_developer_experience_platform_refresh(platform);
}
