/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_completion_provider.c
 *
 * PURPOSE:
 *   Verify completion provider registration, trigger selection, invocation,
 *   resolution, cancellation and registry inventory.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/editor/completion_provider.h"

typedef struct MockProvider {
    size_t provide_calls;
    size_t resolve_calls;
    size_t cancel_calls;
    uint64_t cancelled_request;
} MockProvider;

static void candidate_init(UmiEditorCompletionCandidate *candidate)
{
    (void)memset(candidate, 0, sizeof(*candidate));
    candidate->struct_size = (uint32_t)sizeof(*candidate);
    candidate->api_version = UMI_EDITOR_COMPLETION_CANDIDATE_API_VERSION;
    candidate->item.struct_size = (uint32_t)sizeof(candidate->item);
    candidate->item.api_version = 1U;
    (void)strncpy(candidate->item.id, "candidate",
                  sizeof(candidate->item.id) - 1U);
    (void)strncpy(candidate->item.label, "candidate",
                  sizeof(candidate->item.label) - 1U);
    (void)strncpy(candidate->item.insert_text, "candidate",
                  sizeof(candidate->item.insert_text) - 1U);
    (void)strncpy(candidate->item.kind, "value",
                  sizeof(candidate->item.kind) - 1U);
    (void)strncpy(candidate->provider_id, "provider",
                  sizeof(candidate->provider_id) - 1U);
    candidate->insert_format = UMI_EDITOR_COMPLETION_INSERT_PLAIN_TEXT;
    candidate->source = UMI_EDITOR_COMPLETION_SOURCE_NATIVE_ANALYSER;
}

static UmiStatus capture_candidate(const UmiEditorCompletionCandidate *candidate,
                                   void *user_data)
{
    UmiEditorCompletionCandidate *captured =
        (UmiEditorCompletionCandidate *)user_data;
    *captured = *candidate;
    return UMI_STATUS_OK;
}

static UmiStatus provide(const UmiEditorCompletionRequest *request,
                         UmiEditorCompletionCandidateSink sink,
                         void *sink_user_data,
                         UmiEditorCompletionProviderResponse *out_response,
                         void *provider_user_data)
{
    MockProvider *mock = (MockProvider *)provider_user_data;
    UmiEditorCompletionCandidate candidate;

    assert(request != NULL);
    ++mock->provide_calls;
    candidate_init(&candidate);
    (void)strncpy(candidate.item.document_id, request->document_id,
                  sizeof(candidate.item.document_id) - 1U);
    candidate.document_revision = request->document_revision;
    candidate.request_id = request->request_id;
    assert(sink(&candidate, sink_user_data) == UMI_STATUS_OK);
    out_response->emitted_candidate_count = 1U;
    out_response->provider_revision = 7U;
    out_response->cacheable = 1;
    return UMI_STATUS_OK;
}

static UmiStatus resolve(const UmiEditorCompletionRequest *request,
                         UmiEditorCompletionCandidate *candidate,
                         void *provider_user_data)
{
    MockProvider *mock = (MockProvider *)provider_user_data;
    assert(request != NULL);
    ++mock->resolve_calls;
    (void)strncpy(candidate->documentation, "resolved documentation",
                  sizeof(candidate->documentation) - 1U);
    candidate->flags |= UMI_EDITOR_COMPLETION_CANDIDATE_RESOLVED;
    return UMI_STATUS_OK;
}

static UmiStatus cancel(uint64_t request_id, void *provider_user_data)
{
    MockProvider *mock = (MockProvider *)provider_user_data;
    ++mock->cancel_calls;
    mock->cancelled_request = request_id;
    return UMI_STATUS_OK;
}

int main(void)
{
    UmiEditorCompletionProviderRegistry *registry = NULL;
    UmiEditorCompletionProviderDescriptor descriptor;
    UmiEditorCompletionProviderCallbacks callbacks;
    UmiEditorCompletionProviderRegistrySnapshot snapshot;
    UmiEditorCompletionProviderResponse response;
    UmiEditorCompletionCandidate captured;
    UmiEditorCompletionRequest request;
    MockProvider mock = {0U, 0U, 0U, 0U};

    (void)memset(&descriptor, 0, sizeof(descriptor));
    descriptor.struct_size = (uint32_t)sizeof(descriptor);
    descriptor.api_version = UMI_EDITOR_COMPLETION_PROVIDER_API_VERSION;
    (void)strncpy(descriptor.id, "provider", sizeof(descriptor.id) - 1U);
    (void)strncpy(descriptor.label, "Provider",
                  sizeof(descriptor.label) - 1U);
    (void)strncpy(descriptor.language_id, "c",
                  sizeof(descriptor.language_id) - 1U);
    (void)strncpy(descriptor.trigger_characters, ".>",
                  sizeof(descriptor.trigger_characters) - 1U);
    descriptor.priority = 50;
    descriptor.source = UMI_EDITOR_COMPLETION_SOURCE_NATIVE_ANALYSER;
    descriptor.supports_manual_invocation = 1;
    descriptor.supports_trigger_characters = 1;
    descriptor.supports_automatic_invocation = 1;
    descriptor.supports_incomplete_refresh = 1;
    descriptor.supports_resolution = 1;
    descriptor.supports_commit_characters = 1;
    descriptor.enabled = 1;

    (void)memset(&callbacks, 0, sizeof(callbacks));
    callbacks.struct_size = (uint32_t)sizeof(callbacks);
    callbacks.api_version = UMI_EDITOR_COMPLETION_PROVIDER_API_VERSION;
    callbacks.provide = provide;
    callbacks.resolve = resolve;
    callbacks.cancel = cancel;

    assert(umi_editor_completion_provider_registry_create(&registry) ==
           UMI_STATUS_OK);
    assert(umi_editor_completion_provider_registry_register(registry,
                                                             &descriptor,
                                                             &callbacks,
                                                             &mock) ==
           UMI_STATUS_OK);
    assert(umi_editor_completion_provider_registry_count(registry) == 1U);

    request = umi_editor_completion_request_default("document", "c", 99U);
    request.document_revision = 4U;
    assert(umi_editor_completion_request_validate(&request) == UMI_STATUS_OK);
    assert(umi_editor_completion_provider_supports_request(&descriptor,
                                                            &request));
    request.trigger_kind = UMI_EDITOR_COMPLETION_TRIGGER_CHARACTER;
    (void)strncpy(request.trigger_character, ".",
                  sizeof(request.trigger_character) - 1U);
    assert(umi_editor_completion_provider_supports_request(&descriptor,
                                                            &request));
    (void)strncpy(request.trigger_character, "#",
                  sizeof(request.trigger_character) - 1U);
    assert(!umi_editor_completion_provider_supports_request(&descriptor,
                                                             &request));
    request.trigger_kind = UMI_EDITOR_COMPLETION_TRIGGER_INVOKED;
    request.trigger_character[0] = '\0';

    assert(umi_editor_completion_provider_registry_invoke(registry,
                                                           "provider",
                                                           &request,
                                                           capture_candidate,
                                                           &captured,
                                                           &response) ==
           UMI_STATUS_OK);
    assert(mock.provide_calls == 1U);
    assert(response.emitted_candidate_count == 1U);
    assert(strcmp(captured.item.document_id, "document") == 0);
    assert(umi_editor_completion_provider_registry_resolve(registry,
                                                            "provider",
                                                            &request,
                                                            &captured) ==
           UMI_STATUS_OK);
    assert(mock.resolve_calls == 1U);
    assert((captured.flags & UMI_EDITOR_COMPLETION_CANDIDATE_RESOLVED) != 0U);
    assert(umi_editor_completion_provider_registry_cancel(registry,
                                                           "provider",
                                                           99U) ==
           UMI_STATUS_OK);
    assert(mock.cancel_calls == 1U && mock.cancelled_request == 99U);

    assert(umi_editor_completion_provider_registry_snapshot(registry,
                                                             &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.provider_count == 1U);
    assert(snapshot.enabled_provider_count == 1U);
    assert(snapshot.resolving_provider_count == 1U);
    assert(snapshot.language_count == 1U);

    assert(umi_editor_completion_provider_registry_unregister(registry,
                                                               "provider") ==
           UMI_STATUS_OK);
    assert(umi_editor_completion_provider_registry_count(registry) == 0U);
    umi_editor_completion_provider_registry_destroy(registry);
    return 0;
}
