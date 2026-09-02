/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_ai_coding_assistant.c
 *
 * PURPOSE:
 *   Verify AuthorEngine-governed coding task plans, history and patch review
 *   state through the reusable assistant service.
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

#include "umicom/ai/coding_assistant.h"

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
    UmiAiRuntimeDescriptor descriptor = {0};
    UmiAiContextSource general = {0};
    UmiAiCodingContextFile source = {0};
    UmiAiCodingRequest request;
    UmiAiCodingTaskPlan plan;
    UmiAiCodingPatch patch;
    UmiAiCodingAssistantSnapshot snapshot;

    umi_ai_runtime_init(&runtime);
    (void)strcpy(config.executable, "uaengine");
    (void)strcpy(config.workspace, ".");
    (void)strcpy(config.provider, "authorengine.local");
    assert(umi_ai_authorengine_service_create(&runtime, &config, &authorengine)
           == UMI_STATUS_OK);
    (void)strcpy(descriptor.runtime_id, "authorengine.local.chat");
    (void)strcpy(descriptor.provider_id, "authorengine.local");
    (void)strcpy(descriptor.display_name, "AuthorEngine Local");
    (void)strcpy(descriptor.model_id, "authorengine-local");
    descriptor.provider_kind = UMI_AI_PROVIDER_AUTHOR_ENGINE;
    descriptor.transport = UMI_AI_RUNTIME_PROCESS;
    descriptor.context_tokens = 32768U;
    descriptor.max_output_tokens = 2048U;
    descriptor.configured = 1;
    assert(umi_ai_authorengine_service_register_runtime(authorengine, &descriptor)
           == UMI_STATUS_OK);
    (void)strcpy(general.source_id, "workspace");
    (void)strcpy(general.label, "Workspace");
    (void)strcpy(general.uri, "workspace://root");
    general.kind = UMI_AI_CONTEXT_WORKSPACE;
    general.classification = UMI_AI_DATA_INTERNAL;
    general.estimated_tokens = 200U;
    general.priority = 100U;
    general.mandatory = 1;
    general.enabled = 1;
    assert(umi_ai_context_broker_upsert(
        umi_ai_authorengine_service_context(authorengine), &general)
        == UMI_STATUS_OK);
    assert(umi_ai_coding_assistant_create(authorengine, &assistant)
           == UMI_STATUS_OK);

    (void)strcpy(source.path, "src/app.c");
    (void)strcpy(source.language_id, "c23");
    (void)strcpy(source.summary, "Active application source");
    source.classification = UMI_AI_DATA_INTERNAL;
    source.estimated_tokens = 400U;
    source.priority = 100U;
    source.active = 1;
    source.enabled = 1;
    assert(umi_ai_coding_context_upsert(
        umi_ai_coding_assistant_context(assistant), &source) == UMI_STATUS_OK);

    umi_ai_coding_request_init(&request, UMI_AI_CODING_TASK_GENERATE_TESTS);
    (void)strcpy(request.request_id, "request.48");
    (void)strcpy(request.session_id, "studio.session.default");
    (void)strcpy(request.runtime_id, "authorengine.local.chat");
    (void)strcpy(request.workspace_root, ".");
    (void)strcpy(request.active_path, "src/app.c");
    (void)strcpy(request.language_id, "c23");
    (void)strcpy(request.instruction, "Generate boundary tests.");
    assert(umi_ai_coding_assistant_prepare(
        assistant, &request, UMI_AI_PROVIDER_AUTHOR_ENGINE, &plan)
        == UMI_STATUS_OK);
    assert(plan.ready && plan.repository_context.file_count == 1U);
    assert(plan.general_context_sources == 1U);

    assert(umi_ai_coding_patch_init(
        &patch, "patch.48", request.request_id, "Boundary tests",
        "Cover the public validation contract") == UMI_STATUS_OK);
    assert(umi_ai_coding_patch_add_file(
        &patch, "tests/test_app.c", UMI_AI_CODING_PATCH_CREATE, "",
        "int main(void) { return 0; }\n") == UMI_STATUS_OK);
    assert(umi_ai_coding_assistant_record_patch(assistant, &patch)
           == UMI_STATUS_OK);
    assert(umi_ai_coding_assistant_approve_patch(
        assistant, patch.patch_id, "developer") == UMI_STATUS_OK);
    assert(umi_ai_coding_assistant_snapshot(assistant, &snapshot)
           == UMI_STATUS_OK);
    assert(snapshot.task_count == 1U && snapshot.patch_count == 1U);
    assert(snapshot.approved_patches == 1U);
    umi_ai_coding_assistant_destroy(assistant);
    umi_ai_authorengine_service_destroy(authorengine);
    return 0;
}
