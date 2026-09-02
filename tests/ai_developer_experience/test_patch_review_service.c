/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_developer_experience/test_patch_review_service.c
 *
 * PURPOSE:
 *   Focused regression coverage for AI Developer Experience patch review service.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/ai_developer_experience/patch_review_service.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiAiCodingPatch patch;
    UmiAiDeveloperPatchReviewService service;

    assert(umi_ai_coding_patch_init(
        &patch, "patch.1", "request.1", "Update", "Reason") == UMI_STATUS_OK);
    assert(umi_ai_coding_patch_add_file(
        &patch, "a.c", UMI_AI_CODING_PATCH_MODIFY,
        "a\n", "b\n") == UMI_STATUS_OK);

    umi_ai_developer_patch_review_service_init(&service);
    assert(umi_ai_developer_patch_review_service_load(
        &service, &patch) == UMI_STATUS_OK);
    assert(service.loaded);
    assert(!umi_ai_developer_patch_review_service_ready_to_approve(&service));

    assert(umi_ai_developer_patch_review_service_mark_reviewed(
        &service, 0U, 1) == UMI_STATUS_OK);
    assert(umi_ai_developer_patch_review_service_ready_to_approve(&service));
    return 0;
}

