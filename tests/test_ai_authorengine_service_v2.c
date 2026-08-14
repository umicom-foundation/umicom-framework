/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/test_ai_authorengine_service_v2.c
 *
 * PURPOSE:
 *   Verify the AuthorEngine orchestration service composes reusable AI state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/ai/authorengine_service.h"

static UmiAiRuntimeDescriptor runtime_descriptor(void)
{
    UmiAiRuntimeDescriptor descriptor = {0};
    (void)strcpy(descriptor.runtime_id, "authorengine.local.chat");
    (void)strcpy(descriptor.provider_id, "authorengine.local");
    (void)strcpy(descriptor.display_name, "AuthorEngine Local Chat");
    (void)strcpy(descriptor.model_id, "model.local.chat");
    (void)strcpy(descriptor.endpoint, "process://uaengine");
    descriptor.provider_kind = UMI_AI_PROVIDER_AUTHOR_ENGINE;
    descriptor.transport = UMI_AI_RUNTIME_PROCESS;
    descriptor.capabilities = UMI_AI_RUNTIME_CHAT | UMI_AI_RUNTIME_STREAMING;
    descriptor.context_tokens = 8192U;
    descriptor.max_output_tokens = 1024U;
    descriptor.configured = 1;
    descriptor.local = 1;
    return descriptor;
}

int main(void)
{
    UmiAiRuntime runtime;
    UmiAiAuthorEngineConfig config = {0};
    UmiAiAuthorEngineService *service = NULL;
    UmiAiAuthorEngineServiceSnapshot snapshot;
    UmiAiRuntimeDescriptor descriptor = runtime_descriptor();
    UmiAiContextSource context = {0};
    UmiAiContextPlan plan;
    size_t healthy = 99U;
    umi_ai_runtime_init(&runtime);
    (void)strcpy(config.executable, "uaengine");
    (void)strcpy(config.workspace, ".");
    (void)strcpy(config.provider, "authorengine.local");
    assert(umi_ai_authorengine_service_create(&runtime, &config, &service)
        == UMI_STATUS_OK);
    assert(umi_ai_authorengine_service_register_runtime(service, &descriptor)
        == UMI_STATUS_OK);
    (void)strcpy(context.source_id, "workspace.manifest");
    (void)strcpy(context.label, "Workspace manifest");
    (void)strcpy(context.uri, "workspace://manifest");
    context.kind = UMI_AI_CONTEXT_WORKSPACE;
    context.classification = UMI_AI_DATA_INTERNAL;
    context.estimated_tokens = 1000U;
    context.priority = 100U;
    context.mandatory = 1;
    context.enabled = 1;
    assert(umi_ai_context_broker_upsert(
        umi_ai_authorengine_service_context(service), &context) == UMI_STATUS_OK);
    assert(umi_ai_authorengine_service_begin_session(
        service, "session.service.47", descriptor.provider_id,
        descriptor.model_id, "workspace.batch47", "Service session", 47U)
        == UMI_STATUS_OK);
    assert(umi_ai_authorengine_service_plan_context(
        service, descriptor.runtime_id, 0, &plan) == UMI_STATUS_OK);
    assert(plan.source_count == 1U);
    assert(umi_ai_authorengine_service_refresh_health(service, 48U, &healthy)
        == UMI_STATUS_OK);
    assert(healthy == 0U);
    assert(umi_ai_authorengine_service_snapshot(service, &snapshot)
        == UMI_STATUS_OK);
    assert(snapshot.runtimes == 1U);
    assert(snapshot.sessions == 1U);
    assert(strcmp(snapshot.active_session_id, "session.service.47") == 0);
    umi_ai_authorengine_service_destroy(service);
    return 0;
}
