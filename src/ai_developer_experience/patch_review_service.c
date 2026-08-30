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

void umi_ai_developer_patch_review_service_init(
    UmiAiDeveloperPatchReviewService *service)
{
    if (service == NULL) return;

    (void)memset(service, 0, sizeof(*service));
    service->revision = 1U;
}

UmiStatus umi_ai_developer_patch_review_service_load(
    UmiAiDeveloperPatchReviewService *service,
    const UmiAiCodingPatch *patch)
{
    UmiStatus status;

    if (service == NULL || patch == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    service->patch = *patch;

    status = umi_ai_developer_patch_review_build(
        patch, &service->review);
    if (status != UMI_STATUS_OK) return status;

    service->loaded = 1;
    service->revision += 1U;
    return umi_ai_developer_patch_review_service_refresh_diff(service);
}

UmiStatus umi_ai_developer_patch_review_service_select_file(
    UmiAiDeveloperPatchReviewService *service,
    size_t file_index)
{
    UmiStatus status;

    if (service == NULL || !service->loaded) {
        return UMI_STATUS_INVALID_STATE;
    }

    status = umi_ai_developer_patch_review_select(
        &service->review,
        file_index);
    if (status != UMI_STATUS_OK) return status;

    service->revision += 1U;
    return umi_ai_developer_patch_review_service_refresh_diff(service);
}

UmiStatus umi_ai_developer_patch_review_service_mark_reviewed(
    UmiAiDeveloperPatchReviewService *service,
    size_t file_index,
    int reviewed)
{
    UmiStatus status;

    if (service == NULL || !service->loaded) {
        return UMI_STATUS_INVALID_STATE;
    }

    status = umi_ai_developer_patch_review_mark_reviewed(
        &service->review,
        file_index,
        reviewed);
    if (status == UMI_STATUS_OK) service->revision += 1U;
    return status;
}

UmiStatus umi_ai_developer_patch_review_service_refresh_diff(
    UmiAiDeveloperPatchReviewService *service)
{
    UmiStatus status;

    if (service == NULL || !service->loaded) {
        return UMI_STATUS_INVALID_STATE;
    }

    status = umi_ai_developer_patch_review_diff(
        &service->patch,
        &service->review,
        &service->diff);
    if (status != UMI_STATUS_OK) return status;

    status = umi_ai_developer_side_by_side_build(
        &service->diff,
        &service->split);
    if (status == UMI_STATUS_OK) service->revision += 1U;
    return status;
}

int umi_ai_developer_patch_review_service_ready_to_approve(
    const UmiAiDeveloperPatchReviewService *service)
{
    return service != NULL &&
        service->loaded &&
        service->review.file_count > 0U &&
        service->review.reviewed_file_count ==
            service->review.file_count;
}
