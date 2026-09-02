/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_inline_suggestion.c
 *
 * PURPOSE:
 *   Verify inline suggestion validation, collection snapshots, typed-through
 *   compatibility and provider registration/invocation/cancellation.
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

#include "umicom/editor/inline_suggestion.h"

typedef struct MockInlineProvider {
    size_t provide_calls;
    size_t cancel_calls;
} MockInlineProvider;

/*
 * Initialise suggestion from caller-provided values so later operations receive a known
 * state.
 */
static void suggestion_init(UmiEditorInlineSuggestion *suggestion,
                            const char *provider,
                            const char *id,
                            const char *text)
{
    (void)memset(suggestion, 0, sizeof(*suggestion));
    suggestion->struct_size = (uint32_t)sizeof(*suggestion);
    suggestion->api_version = UMI_EDITOR_INLINE_SUGGESTION_API_VERSION;
    (void)strncpy(suggestion->provider_id, provider,
                  sizeof(suggestion->provider_id) - 1U);
    (void)strncpy(suggestion->id, id, sizeof(suggestion->id) - 1U);
    (void)strncpy(suggestion->document_id, "document",
                  sizeof(suggestion->document_id) - 1U);
    (void)strncpy(suggestion->label, id,
                  sizeof(suggestion->label) - 1U);
    (void)strncpy(suggestion->text, text,
                  sizeof(suggestion->text) - 1U);
    suggestion->replace_start_offset = 5U;
    suggestion->replace_end_offset = 5U;
    suggestion->document_revision = 8U;
    suggestion->request_id = 71U;
    suggestion->confidence_milli = 900;
    suggestion->source = UMI_EDITOR_COMPLETION_SOURCE_NATIVE_ANALYSER;
    suggestion->flags = UMI_EDITOR_INLINE_SUGGESTION_COMPLETE;
}

/*
 * Exercise capture and return a clear result when the behaviour no longer matches its
 * contract.
 */
static UmiStatus capture(const UmiEditorInlineSuggestion *suggestion,
                         void *user_data)
{
    *(UmiEditorInlineSuggestion *)user_data = *suggestion;
    return UMI_STATUS_OK;
}

/*
 * Exercise provide and return a clear result when the behaviour no longer matches its
 * contract.
 */
static UmiStatus provide(const UmiEditorCompletionRequest *request,
                         UmiEditorInlineSuggestionSink sink,
                         void *sink_user_data,
                         UmiEditorInlineSuggestionProviderResponse *out_response,
                         void *provider_user_data)
{
    MockInlineProvider *mock = (MockInlineProvider *)provider_user_data;
    UmiEditorInlineSuggestion suggestion;

    ++mock->provide_calls;
    suggestion_init(&suggestion, "inline", "suggestion", "return value;");
    suggestion.document_revision = request->document_revision;
    suggestion.request_id = request->request_id;
    assert(sink(&suggestion, sink_user_data) == UMI_STATUS_OK);
    out_response->emitted_suggestion_count = 1U;
    out_response->provider_revision = 3U;
    return UMI_STATUS_OK;
}

/*
 * Exercise cancel and return a clear result when the behaviour no longer matches its
 * contract.
 */
static UmiStatus cancel(uint64_t request_id, void *provider_user_data)
{
    MockInlineProvider *mock = (MockInlineProvider *)provider_user_data;
    assert(request_id == 71U);
    ++mock->cancel_calls;
    return UMI_STATUS_OK;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiEditorInlineSuggestionCollection *collection = NULL;
    UmiEditorInlineSuggestionProviderRegistry *registry = NULL;
    UmiEditorInlineSuggestion suggestion;
    UmiEditorInlineSuggestion ai;
    UmiEditorInlineSuggestion captured;
    UmiEditorInlineSuggestionCollectionSnapshot collection_snapshot;
    UmiEditorInlineSuggestionProviderRegistrySnapshot provider_snapshot;
    UmiEditorInlineSuggestionProviderDescriptor descriptor;
    UmiEditorInlineSuggestionProviderCallbacks callbacks;
    UmiEditorInlineSuggestionProviderResponse response;
    UmiEditorCompletionRequest request;
    MockInlineProvider mock = {0U, 0U};

    suggestion_init(&suggestion, "native", "one", "return value;");
    assert(umi_editor_inline_suggestion_validate(&suggestion) == UMI_STATUS_OK);
    assert(umi_editor_inline_suggestion_accepts_typed_prefix(&suggestion,
                                                             "ret"));
    assert(!umi_editor_inline_suggestion_accepts_typed_prefix(&suggestion,
                                                              "let"));
    assert(umi_editor_inline_suggestion_collection_create(&collection) ==
           UMI_STATUS_OK);
    assert(umi_editor_inline_suggestion_collection_upsert(collection,
                                                           &suggestion) ==
           UMI_STATUS_OK);
    ai = suggestion;
    (void)strncpy(ai.provider_id, "ai", sizeof(ai.provider_id) - 1U);
    (void)strncpy(ai.id, "two", sizeof(ai.id) - 1U);
    ai.source = UMI_EDITOR_COMPLETION_SOURCE_AI;
    ai.flags = UMI_EDITOR_INLINE_SUGGESTION_REMOTE |
               UMI_EDITOR_INLINE_SUGGESTION_STREAMING;
    assert(umi_editor_inline_suggestion_collection_upsert(collection, &ai) ==
           UMI_STATUS_OK);
    assert(umi_editor_inline_suggestion_collection_snapshot(
               collection, &collection_snapshot) == UMI_STATUS_OK);
    assert(collection_snapshot.suggestion_count == 2U);
    assert(collection_snapshot.provider_count == 2U);
    assert(collection_snapshot.ai_count == 1U);
    assert(collection_snapshot.remote_count == 1U);
    assert(collection_snapshot.streaming_count == 1U);

    (void)memset(&descriptor, 0, sizeof(descriptor));
    descriptor.struct_size = (uint32_t)sizeof(descriptor);
    descriptor.api_version = UMI_EDITOR_INLINE_SUGGESTION_API_VERSION;
    (void)strncpy(descriptor.id, "inline", sizeof(descriptor.id) - 1U);
    (void)strncpy(descriptor.label, "Inline Provider",
                  sizeof(descriptor.label) - 1U);
    (void)strncpy(descriptor.language_id, "c",
                  sizeof(descriptor.language_id) - 1U);
    descriptor.priority = 50;
    descriptor.source = UMI_EDITOR_COMPLETION_SOURCE_NATIVE_ANALYSER;
    descriptor.supports_manual_invocation = 1;
    descriptor.supports_automatic_invocation = 1;
    descriptor.supports_streaming = 1;
    descriptor.supports_partial_acceptance = 1;
    descriptor.enabled = 1;
    (void)memset(&callbacks, 0, sizeof(callbacks));
    callbacks.struct_size = (uint32_t)sizeof(callbacks);
    callbacks.api_version = UMI_EDITOR_INLINE_SUGGESTION_API_VERSION;
    callbacks.provide = provide;
    callbacks.cancel = cancel;

    assert(umi_editor_inline_suggestion_provider_registry_create(&registry) ==
           UMI_STATUS_OK);
    assert(umi_editor_inline_suggestion_provider_registry_register(registry,
                                                                    &descriptor,
                                                                    &callbacks,
                                                                    &mock) ==
           UMI_STATUS_OK);
    request = umi_editor_completion_request_default("document", "c", 71U);
    request.document_revision = 8U;
    assert(umi_editor_inline_suggestion_provider_registry_invoke(registry,
                                                                  "inline",
                                                                  &request,
                                                                  capture,
                                                                  &captured,
                                                                  &response) ==
           UMI_STATUS_OK);
    assert(mock.provide_calls == 1U);
    assert(response.emitted_suggestion_count == 1U);
    assert(strcmp(captured.text, "return value;") == 0);
    assert(umi_editor_inline_suggestion_provider_registry_cancel(registry,
                                                                  "inline",
                                                                  71U) ==
           UMI_STATUS_OK);
    assert(mock.cancel_calls == 1U);
    assert(umi_editor_inline_suggestion_provider_registry_snapshot(
               registry, &provider_snapshot) == UMI_STATUS_OK);
    assert(provider_snapshot.provider_count == 1U);
    assert(provider_snapshot.streaming_provider_count == 1U);

    umi_editor_inline_suggestion_provider_registry_destroy(registry);
    umi_editor_inline_suggestion_collection_destroy(collection);
    return 0;
}
