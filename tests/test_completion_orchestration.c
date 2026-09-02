/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_completion_orchestration.c
 *
 * PURPOSE:
 *   Verify completion provider aggregation, AI/remote policy, continuation on
 *   provider failure, deterministic results, selected-item resolution and
 *   cancellation propagation.
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

#include "umicom/editor/completion_orchestration.h"

typedef struct MockProvider {
    const char *label;
    UmiStatus provide_status;
    size_t provide_calls;
    size_t resolve_calls;
    size_t cancel_calls;
} MockProvider;

/*
 * Exercise provide and return a clear result when the behaviour no longer matches its
 * contract.
 */
static UmiStatus provide(const UmiEditorCompletionRequest *request,
                         UmiEditorCompletionCandidateSink sink,
                         void *sink_user_data,
                         UmiEditorCompletionProviderResponse *out_response,
                         void *provider_user_data)
{
    MockProvider *mock = (MockProvider *)provider_user_data;
    UmiEditorCompletionCandidate candidate;

    ++mock->provide_calls;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (mock->provide_status != UMI_STATUS_OK) return mock->provide_status;
    (void)memset(&candidate, 0, sizeof(candidate));
    candidate.struct_size = (uint32_t)sizeof(candidate);
    candidate.api_version = UMI_EDITOR_COMPLETION_CANDIDATE_API_VERSION;
    candidate.item.struct_size = (uint32_t)sizeof(candidate.item);
    candidate.item.api_version = 1U;
    (void)strncpy(candidate.item.id, mock->label,
                  sizeof(candidate.item.id) - 1U);
    (void)strncpy(candidate.item.label, mock->label,
                  sizeof(candidate.item.label) - 1U);
    (void)strncpy(candidate.item.insert_text, mock->label,
                  sizeof(candidate.item.insert_text) - 1U);
    (void)strncpy(candidate.item.kind, "function",
                  sizeof(candidate.item.kind) - 1U);
    candidate.replace_start_offset = request->cursor_offset;
    candidate.replace_end_offset = request->cursor_offset;
    candidate.insert_format = UMI_EDITOR_COMPLETION_INSERT_PLAIN_TEXT;
    candidate.source = UMI_EDITOR_COMPLETION_SOURCE_OTHER;
    assert(sink(&candidate, sink_user_data) == UMI_STATUS_OK);
    out_response->emitted_candidate_count = 1U;
    out_response->provider_revision = 12U;
    return UMI_STATUS_OK;
}

/*
 * Exercise resolve and return a clear result when the behaviour no longer matches its
 * contract.
 */
static UmiStatus resolve(const UmiEditorCompletionRequest *request,
                         UmiEditorCompletionCandidate *candidate,
                         void *provider_user_data)
{
    MockProvider *mock = (MockProvider *)provider_user_data;
    assert(request->request_id != 0U);
    ++mock->resolve_calls;
    (void)strncpy(candidate->documentation, "resolved",
                  sizeof(candidate->documentation) - 1U);
    return UMI_STATUS_OK;
}

/*
 * Exercise cancel and return a clear result when the behaviour no longer matches its
 * contract.
 */
static UmiStatus cancel(uint64_t request_id, void *provider_user_data)
{
    MockProvider *mock = (MockProvider *)provider_user_data;
    assert(request_id != 0U);
    ++mock->cancel_calls;
    return UMI_STATUS_OK;
}

/*
 * Exercise register provider and return a clear result when the behaviour no longer
 * matches its contract.
 */
static void register_provider(UmiEditorCompletionProviderRegistry *registry,
                              const char *id,
                              int32_t priority,
                              UmiEditorCompletionSource source,
                              int remote,
                              MockProvider *mock)
{
    UmiEditorCompletionProviderDescriptor descriptor;
    UmiEditorCompletionProviderCallbacks callbacks;

    (void)memset(&descriptor, 0, sizeof(descriptor));
    descriptor.struct_size = (uint32_t)sizeof(descriptor);
    descriptor.api_version = UMI_EDITOR_COMPLETION_PROVIDER_API_VERSION;
    (void)strncpy(descriptor.id, id, sizeof(descriptor.id) - 1U);
    (void)strncpy(descriptor.label, id, sizeof(descriptor.label) - 1U);
    (void)strncpy(descriptor.language_id, "c",
                  sizeof(descriptor.language_id) - 1U);
    descriptor.priority = priority;
    descriptor.source = source;
    descriptor.supports_manual_invocation = 1;
    descriptor.supports_resolution = 1;
    descriptor.requires_network = remote;
    descriptor.enabled = 1;

    (void)memset(&callbacks, 0, sizeof(callbacks));
    callbacks.struct_size = (uint32_t)sizeof(callbacks);
    callbacks.api_version = UMI_EDITOR_COMPLETION_PROVIDER_API_VERSION;
    callbacks.provide = provide;
    callbacks.resolve = resolve;
    callbacks.cancel = cancel;
    assert(umi_editor_completion_provider_registry_register(registry,
                                                             &descriptor,
                                                             &callbacks,
                                                             mock) ==
           UMI_STATUS_OK);
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiEditorCompletionProviderRegistry *registry = NULL;
    UmiEditorCompletionOrchestration *orchestration = NULL;
    UmiEditorCompletionRequest request;
    UmiEditorCompletionRankingConfig ranking;
    UmiEditorCompletionPolicy policy;
    UmiEditorCompletionOrchestrationSnapshot snapshot;
    UmiEditorCompletionProviderReport report;
    UmiEditorCompletionCandidate selected;
    MockProvider native = {"printf", UMI_STATUS_OK, 0U, 0U, 0U};
    MockProvider failing = {"failure", UMI_STATUS_INTERNAL_ERROR, 0U, 0U, 0U};
    MockProvider ai = {"printAI", UMI_STATUS_OK, 0U, 0U, 0U};

    assert(umi_editor_completion_provider_registry_create(&registry) ==
           UMI_STATUS_OK);
    register_provider(registry, "native", 90,
                      UMI_EDITOR_COMPLETION_SOURCE_NATIVE_ANALYSER, 0, &native);
    register_provider(registry, "failing", 60,
                      UMI_EDITOR_COMPLETION_SOURCE_NATIVE_ANALYSER, 0, &failing);
    register_provider(registry, "ai", 100,
                      UMI_EDITOR_COMPLETION_SOURCE_AI, 1, &ai);

    assert(umi_editor_completion_orchestration_create(registry,
                                                       &orchestration) ==
           UMI_STATUS_OK);
    policy = umi_editor_completion_policy_default();
    policy.allow_ai = 1;
    policy.allow_remote = 1;
    policy.continue_on_provider_error = 1;
    assert(umi_editor_completion_orchestration_set_policy(orchestration,
                                                           &policy) ==
           UMI_STATUS_OK);
    request = umi_editor_completion_request_default("document", "c", 44U);
    request.allow_ai = 0;
    request.allow_remote = 0;
    (void)strncpy(request.prefix, "pri", sizeof(request.prefix) - 1U);
    request.cursor_offset = 5U;
    request.document_revision = 3U;
    ranking = umi_editor_completion_ranking_config_default();
    assert(umi_editor_completion_orchestration_begin(orchestration,
                                                      &request,
                                                      &ranking) ==
           UMI_STATUS_OK);
    assert(umi_editor_completion_orchestration_snapshot(orchestration,
                                                         &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.state == UMI_EDITOR_COMPLETION_ORCHESTRATION_READY);
    assert(snapshot.eligible_provider_count == 3U);
    assert(snapshot.invoked_provider_count == 2U);
    assert(snapshot.failed_provider_count == 1U);
    assert(snapshot.collected_candidate_count == 1U);
    assert(snapshot.result_count == 1U);
    assert(native.provide_calls == 1U);
    assert(failing.provide_calls == 1U);
    assert(ai.provide_calls == 0U);

    assert(umi_editor_completion_orchestration_report_at(orchestration,
                                                          2U,
                                                          &report) ==
           UMI_STATUS_OK);
    assert(strcmp(report.provider_id, "ai") == 0);
    assert(report.skipped_by_policy);

    assert(umi_editor_completion_orchestration_resolve_selected(
               orchestration) == UMI_STATUS_OK);
    assert(native.resolve_calls == 1U);
    assert(umi_editor_completion_session_selected(
               umi_editor_completion_orchestration_session(orchestration),
               &selected) == UMI_STATUS_OK);
    assert(strcmp(selected.documentation, "resolved") == 0);
    assert((selected.flags & UMI_EDITOR_COMPLETION_CANDIDATE_RESOLVED) != 0U);

    assert(umi_editor_completion_orchestration_cancel(orchestration) ==
           UMI_STATUS_OK);
    assert(native.cancel_calls == 1U);
    assert(failing.cancel_calls == 1U);
    assert(ai.cancel_calls == 0U);

    umi_editor_completion_orchestration_destroy(orchestration);
    umi_editor_completion_provider_registry_destroy(registry);
    return 0;
}
