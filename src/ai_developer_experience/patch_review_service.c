/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_developer_experience/patch_review_service.c
 *
 * PURPOSE:
 *   Implement stateful review selection and diff refresh.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_developer_experience/patch_review_service.h"

#include <string.h>

/*
 * Initialise ai developer patch review service from caller-provided values so later
 * operations receive a known state.
 */
void umi_ai_developer_patch_review_service_init(
    UmiAiDeveloperPatchReviewService *service)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL) return;

    (void)memset(service, 0, sizeof(*service));
    service->revision = 1U;
}

/*
 * Read ai developer patch review service into validated module state and return a status
 * when input cannot be used.
 */
UmiStatus umi_ai_developer_patch_review_service_load(
    UmiAiDeveloperPatchReviewService *service,
    const UmiAiCodingPatch *patch)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL || patch == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    service->patch = *patch;

    status = umi_ai_developer_patch_review_build(
        patch, &service->review);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    service->loaded = 1;
    service->revision += 1U;
    return umi_ai_developer_patch_review_service_refresh_diff(service);
}

/*
 * Provide the ai developer patch review service select file operation used by this module
 * and its client applications.
 */
UmiStatus umi_ai_developer_patch_review_service_select_file(
    UmiAiDeveloperPatchReviewService *service,
    size_t file_index)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL || !service->loaded) {
        return UMI_STATUS_INVALID_STATE;
    }

    status = umi_ai_developer_patch_review_select(
        &service->review,
        file_index);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    service->revision += 1U;
    return umi_ai_developer_patch_review_service_refresh_diff(service);
}

/*
 * Provide the ai developer patch review service mark reviewed operation used by this
 * module and its client applications.
 */
UmiStatus umi_ai_developer_patch_review_service_mark_reviewed(
    UmiAiDeveloperPatchReviewService *service,
    size_t file_index,
    int reviewed)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL || !service->loaded) {
        return UMI_STATUS_INVALID_STATE;
    }

    status = umi_ai_developer_patch_review_mark_reviewed(
        &service->review,
        file_index,
        reviewed);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) service->revision += 1U;
    return status;
}

/*
 * Provide the ai developer patch review service refresh diff operation used by this module
 * and its client applications.
 */
UmiStatus umi_ai_developer_patch_review_service_refresh_diff(
    UmiAiDeveloperPatchReviewService *service)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL || !service->loaded) {
        return UMI_STATUS_INVALID_STATE;
    }

    status = umi_ai_developer_patch_review_diff(
        &service->patch,
        &service->review,
        &service->diff);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_ai_developer_side_by_side_build(
        &service->diff,
        &service->split);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) service->revision += 1U;
    return status;
}

/*
 * Provide the ai developer patch review service ready to approve operation used by this
 * module and its client applications.
 */
int umi_ai_developer_patch_review_service_ready_to_approve(
    const UmiAiDeveloperPatchReviewService *service)
{
    return service != NULL &&
        service->loaded &&
        service->review.file_count > 0U &&
        service->review.reviewed_file_count ==
            service->review.file_count;
}
