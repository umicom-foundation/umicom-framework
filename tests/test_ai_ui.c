/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/test_ai_ui.c
 *
 * PURPOSE:
 *   Verify every toolkit-neutral AuthorEngine surface can project service state.
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

#include "umicom/ai_ui/ai_ui.h"
#include "umicom/ui/command_view.h"

int main(void)
{
    UmiAiRuntime runtime;
    UmiAiAuthorEngineConfig config = {0};
    UmiAiAuthorEngineService *service = NULL;
    UmiAiRuntimeDescriptor descriptor = {0};
    UmiUiViewModel *view = NULL;
    UmiUiValue value;
    UmiUiCommandViewAction action;
    umi_ai_runtime_init(&runtime);
    (void)strcpy(config.executable, "uaengine");
    (void)strcpy(config.workspace, ".");
    (void)strcpy(config.provider, "authorengine.local");
    assert(umi_ai_authorengine_service_create(&runtime, &config, &service)
        == UMI_STATUS_OK);
    (void)strcpy(descriptor.runtime_id, "authorengine.local.chat");
    (void)strcpy(descriptor.provider_id, "authorengine.local");
    (void)strcpy(descriptor.display_name, "AuthorEngine Local Chat");
    (void)strcpy(descriptor.model_id, "model.local.chat");
    descriptor.provider_kind = UMI_AI_PROVIDER_AUTHOR_ENGINE;
    descriptor.transport = UMI_AI_RUNTIME_PROCESS;
    descriptor.capabilities = UMI_AI_RUNTIME_CHAT;
    descriptor.context_tokens = 8192U;
    descriptor.max_output_tokens = 1024U;
    descriptor.configured = 1;
    descriptor.local = 1;
    assert(umi_ai_authorengine_service_register_runtime(service, &descriptor)
        == UMI_STATUS_OK);
    assert(umi_ai_ui_authorengine_overview_view_create(
        "studio.ai-workspace", service, &view) == UMI_STATUS_OK);
    assert(umi_ui_view_model_get_property(
        view, "authorengine.runtimes", &value) == UMI_STATUS_OK);
    assert(value.integer_value == 1);
    assert(umi_ui_command_view_action_at(view, 6U, &action) == UMI_STATUS_OK);
    assert(action.enabled);
    umi_ui_view_model_destroy(view);
    assert(umi_ai_ui_runtime_catalogue_view_create(
        "studio.ai-runtimes", service, &view) == UMI_STATUS_OK);
    umi_ui_view_model_destroy(view);
    assert(umi_ai_ui_context_view_create(
        "studio.ai-context", service, &view) == UMI_STATUS_OK);
    umi_ui_view_model_destroy(view);
    assert(umi_ai_ui_sessions_view_create(
        "studio.ai-sessions", service, &view) == UMI_STATUS_OK);
    umi_ui_view_model_destroy(view);
    assert(umi_ai_ui_privacy_view_create(
        "studio.ai-privacy", service, &view) == UMI_STATUS_OK);
    umi_ui_view_model_destroy(view);
    umi_ai_authorengine_service_destroy(service);
    return 0;
}
