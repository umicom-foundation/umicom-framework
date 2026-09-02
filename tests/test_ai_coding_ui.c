/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_ai_coding_ui.c
 *
 * PURPOSE:
 *   Verify toolkit-neutral AI coding assistant, repository and empty patch
 *   review view-model projections.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/ai_ui/coding_assistant_views.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiAiRuntime runtime;
    UmiAiAuthorEngineConfig config = {0};
    UmiAiAuthorEngineService *authorengine = NULL;
    UmiAiCodingAssistantService *assistant = NULL;
    UmiUiViewModel *view = NULL;
    UmiUiValue value;

    umi_ai_runtime_init(&runtime);
    (void)strcpy(config.executable, "uaengine");
    (void)strcpy(config.workspace, ".");
    (void)strcpy(config.provider, "authorengine.local");
    assert(umi_ai_authorengine_service_create(&runtime, &config, &authorengine)
           == UMI_STATUS_OK);
    assert(umi_ai_coding_assistant_create(authorengine, &assistant)
           == UMI_STATUS_OK);
    assert(umi_ai_ui_coding_assistant_view_create(
        "coding", assistant, &view) == UMI_STATUS_OK);
    assert(umi_ui_view_model_get_property(
        view, "ai-coding.approval-required", &value) == UMI_STATUS_OK);
    assert(value.boolean_value);
    umi_ui_view_model_destroy(view);
    view = NULL;
    assert(umi_ai_ui_coding_patch_view_create(
        "patch", assistant, "", &view) == UMI_STATUS_OK);
    assert(umi_ui_view_model_get_property(
        view, "ai-coding-patch.state", &value) == UMI_STATUS_OK);
    assert(strcmp(value.string_value, "no patch") == 0);
    umi_ui_view_model_destroy(view);
    umi_ai_coding_assistant_destroy(assistant);
    umi_ai_authorengine_service_destroy(authorengine);
    return 0;
}
