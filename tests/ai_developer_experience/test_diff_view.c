/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_developer_experience/test_diff_view.c
 *
 * PURPOSE:
 *   Toolkit-neutral view coverage for AI Developer Experience diff view.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/ai_developer_experience/views/diff.h"
#include "umicom/ui/view_model.h"

int main(void)
{
    UmiAiCodingPatch patch;
    UmiAiDeveloperPatchReviewService review;
    UmiAiDeveloperPreferences preferences;
    UmiUiViewModel *view = NULL;
    UmiUiValue value;

    assert(umi_ai_coding_patch_init(
        &patch, "patch.1", "request.1", "Update", "Reason") == UMI_STATUS_OK);
    assert(umi_ai_coding_patch_add_file(
        &patch, "a.c", UMI_AI_CODING_PATCH_MODIFY,
        "one\ntwo\n", "one\nTWO\n") == UMI_STATUS_OK);

    umi_ai_developer_patch_review_service_init(&review);
    assert(umi_ai_developer_patch_review_service_load(
        &review, &patch) == UMI_STATUS_OK);
    umi_ai_developer_preferences_init(&preferences);

    assert(umi_ai_developer_diff_view_create(
        "test.diff", &review, &preferences, 0U, &view) == UMI_STATUS_OK);
    assert(umi_ui_view_model_get_property(
        view, "ai-diff.layout", &value) == UMI_STATUS_OK);
    assert(value.kind == UMI_UI_VALUE_STRING);
    assert(strcmp(value.string_value, "side-by-side") == 0);

    umi_ui_view_model_destroy(view);
    return 0;
}

