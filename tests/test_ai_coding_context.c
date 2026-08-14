/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_ai_coding_context.c
 *
 * PURPOSE:
 *   Verify deterministic active-file priority, token limits and remote privacy
 *   filtering for repository context plans.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/ai/coding_context.h"

static UmiAiCodingContextFile file(const char *path,
                                   uint32_t tokens,
                                   uint32_t priority,
                                   int active,
                                   UmiAiDataClassification classification)
{
    UmiAiCodingContextFile value = {0};
    (void)strcpy(value.path, path);
    (void)strcpy(value.language_id, "c23");
    (void)strcpy(value.summary, "Repository source file");
    value.estimated_tokens = tokens;
    value.priority = priority;
    value.active = active;
    value.enabled = 1;
    value.classification = classification;
    return value;
}

int main(void)
{
    UmiAiCodingContextIndex *index = NULL;
    UmiAiCodingContextPlan plan;
    UmiAiPrivacyPolicy privacy = umi_ai_privacy_policy_default();
    UmiAiCodingContextFile active = file(
        "src/active.c", 300U, 10U, 1, UMI_AI_DATA_INTERNAL);
    UmiAiCodingContextFile header = file(
        "include/api.h", 250U, 90U, 0, UMI_AI_DATA_PUBLIC);
    UmiAiCodingContextFile secret = file(
        "config/secret.c", 100U, 100U, 0, UMI_AI_DATA_CONFIDENTIAL);

    assert(umi_ai_coding_context_index_create(&index) == UMI_STATUS_OK);
    assert(umi_ai_coding_context_upsert(index, &header) == UMI_STATUS_OK);
    assert(umi_ai_coding_context_upsert(index, &secret) == UMI_STATUS_OK);
    assert(umi_ai_coding_context_upsert(index, &active) == UMI_STATUS_OK);
    assert(umi_ai_coding_context_plan(
        index, 600U, 3U, UMI_AI_PROVIDER_AUTHOR_ENGINE, &privacy, 0, &plan)
        == UMI_STATUS_OK);
    assert(plan.file_count == 2U);
    assert(strcmp(plan.files[0].path, "src/active.c") == 0);
    assert(strcmp(plan.files[1].path, "include/api.h") == 0);
    assert(plan.dropped_count == 1U);
    assert(plan.truncated);
    assert(plan.selected_tokens == 550U);
    assert(umi_ai_coding_context_plan(
        index, 600U, 3U, UMI_AI_PROVIDER_REMOTE, &privacy, 0, &plan)
        == UMI_STATUS_OK);
    assert(plan.file_count == 1U);
    assert(strcmp(plan.files[0].path, "include/api.h") == 0);
    umi_ai_coding_context_index_destroy(index);
    return 0;
}
