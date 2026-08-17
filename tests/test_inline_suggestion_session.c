/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_inline_suggestion_session.c
 *
 * PURPOSE:
 *   Verify inline provider policy, deterministic selection, streaming updates,
 *   typed-through handling and revision-safe word/full acceptance.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/editor/inline_suggestion_session.h"

typedef struct MockInlineProvider {
    const char *text;
    int streaming;
    size_t provide_calls;
    size_t cancel_calls;
} MockInlineProvider;

static UmiStatus provide(const UmiEditorCompletionRequest *request,
                         UmiEditorInlineSuggestionSink sink,
                         void *sink_user_data,
                         UmiEditorInlineSuggestionProviderResponse *out_response,
                         void *provider_user_data)
{
    MockInlineProvider *mock = (MockInlineProvider *)provider_user_data;
    UmiEditorInlineSuggestion suggestion;

    ++mock->provide_calls;
    (void)memset(&suggestion, 0, sizeof(suggestion));
    suggestion.struct_size = (uint32_t)sizeof(suggestion);
    suggestion.api_version = UMI_EDITOR_INLINE_SUGGESTION_API_VERSION;
    (void)strncpy(suggestion.id, "suggestion",
                  sizeof(suggestion.id) - 1U);
    (void)strncpy(suggestion.document_id, request->document_id,
                  sizeof(suggestion.document_id) - 1U);
    (void)strncpy(suggestion.label, "Inline suggestion",
                  sizeof(suggestion.label) - 1U);
    (void)strncpy(suggestion.text, mock->text,
                  sizeof(suggestion.text) - 1U);
    suggestion.replace_start_offset = request->cursor_offset;
    suggestion.replace_end_offset = request->cursor_offset;
    suggestion.document_revision = request->document_revision;
    suggestion.request_id = request->request_id;
    suggestion.confidence_milli = 920;
    suggestion.source = UMI_EDITOR_COMPLETION_SOURCE_OTHER;
    suggestion.flags = mock->streaming
        ? UMI_EDITOR_INLINE_SUGGESTION_STREAMING
        : UMI_EDITOR_INLINE_SUGGESTION_COMPLETE;
    assert(sink(&suggestion, sink_user_data) == UMI_STATUS_OK);
    out_response->emitted_suggestion_count = 1U;
    out_response->provider_revision = 5U;
    out_response->streaming = mock->streaming;
    out_response->incomplete = mock->streaming;
    return UMI_STATUS_OK;
}

static UmiStatus cancel(uint64_t request_id, void *provider_user_data)
{
    MockInlineProvider *mock = (MockInlineProvider *)provider_user_data;
    assert(request_id != 0U);
    ++mock->cancel_calls;
    return UMI_STATUS_OK;
}

static void register_provider(
    UmiEditorInlineSuggestionProviderRegistry *registry,
    const char *id,
    int32_t priority,
    UmiEditorCompletionSource source,
    int remote,
    int streaming,
    MockInlineProvider *mock)
{
    UmiEditorInlineSuggestionProviderDescriptor descriptor;
    UmiEditorInlineSuggestionProviderCallbacks callbacks;

    (void)memset(&descriptor, 0, sizeof(descriptor));
    descriptor.struct_size = (uint32_t)sizeof(descriptor);
    descriptor.api_version = UMI_EDITOR_INLINE_SUGGESTION_API_VERSION;
    (void)strncpy(descriptor.id, id, sizeof(descriptor.id) - 1U);
    (void)strncpy(descriptor.label, id, sizeof(descriptor.label) - 1U);
    (void)strncpy(descriptor.language_id, "c",
                  sizeof(descriptor.language_id) - 1U);
    descriptor.priority = priority;
    descriptor.source = source;
    descriptor.supports_manual_invocation = 1;
    descriptor.supports_automatic_invocation = 1;
    descriptor.supports_streaming = streaming;
    descriptor.supports_partial_acceptance = 1;
    descriptor.requires_network = remote;
    descriptor.enabled = 1;

    (void)memset(&callbacks, 0, sizeof(callbacks));
    callbacks.struct_size = (uint32_t)sizeof(callbacks);
    callbacks.api_version = UMI_EDITOR_INLINE_SUGGESTION_API_VERSION;
    callbacks.provide = provide;
    callbacks.cancel = cancel;
    assert(umi_editor_inline_suggestion_provider_registry_register(registry,
                                                                    &descriptor,
                                                                    &callbacks,
                                                                    mock) ==
           UMI_STATUS_OK);
}

int main(void)
{
    UmiEditorInlineSuggestionProviderRegistry *registry = NULL;
    UmiEditorInlineSuggestionSession *session = NULL;
    UmiEditorTextBuffer *buffer = NULL;
    UmiEditorCompletionRequest request;
    UmiEditorInlineSuggestionPolicy policy;
    UmiEditorInlineSuggestionSessionSnapshot snapshot;
    UmiEditorInlineSuggestionProviderReport report;
    UmiEditorInlineSuggestion current;
    UmiEditorInlineSuggestionAcceptance acceptance;
    UmiEditorTextBufferView view;
    uint64_t revision;
    MockInlineProvider local = {"ret", 1, 0U, 0U};
    MockInlineProvider ai = {"remote suggestion", 0, 0U, 0U};

    assert(umi_editor_inline_suggestion_provider_registry_create(&registry) ==
           UMI_STATUS_OK);
    register_provider(registry, "local", 80,
                      UMI_EDITOR_COMPLETION_SOURCE_NATIVE_ANALYSER,
                      0, 1, &local);
    register_provider(registry, "ai", 100,
                      UMI_EDITOR_COMPLETION_SOURCE_AI,
                      1, 0, &ai);
    assert(umi_editor_inline_suggestion_session_create(registry, &session) ==
           UMI_STATUS_OK);
    policy = umi_editor_inline_suggestion_policy_default();
    policy.allow_ai = 1;
    policy.allow_remote = 1;
    assert(umi_editor_inline_suggestion_session_set_policy(session, &policy) ==
           UMI_STATUS_OK);

    assert(umi_editor_text_buffer_create(32U, &buffer) == UMI_STATUS_OK);
    assert(umi_editor_text_buffer_set(buffer, "x", 1U) == UMI_STATUS_OK);
    revision = umi_editor_text_buffer_revision(buffer);
    request = umi_editor_completion_request_default("document", "c", 81U);
    request.allow_ai = 0;
    request.allow_remote = 0;
    request.cursor_offset = 1U;
    request.document_revision = revision;
    assert(umi_editor_inline_suggestion_session_begin(session, &request) ==
           UMI_STATUS_OK);
    assert(umi_editor_inline_suggestion_session_snapshot(session, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.state == UMI_EDITOR_INLINE_SUGGESTION_SESSION_STREAMING);
    assert(snapshot.suggestion_count == 1U);
    assert(snapshot.invoked_provider_count == 1U);
    assert(local.provide_calls == 1U);
    assert(ai.provide_calls == 0U);
    assert(umi_editor_inline_suggestion_session_report_at(session, 1U,
                                                           &report) ==
           UMI_STATUS_OK);
    assert(strcmp(report.provider_id, "ai") == 0);
    assert(report.skipped_by_policy);

    assert(umi_editor_inline_suggestion_session_append_chunk(session,
                                                              "local",
                                                              "suggestion",
                                                              "urn value;",
                                                              1) ==
           UMI_STATUS_OK);
    assert(umi_editor_inline_suggestion_session_current(session, &current) ==
           UMI_STATUS_OK);
    assert(strcmp(current.text, "return value;") == 0);
    assert(umi_editor_inline_suggestion_session_accept(
               session,
               buffer,
               revision,
               UMI_EDITOR_INLINE_SUGGESTION_ACCEPT_WORD,
               &acceptance) == UMI_STATUS_OK);
    assert(strcmp(acceptance.inserted_text, "return") == 0);
    assert(!acceptance.complete);
    assert(umi_editor_text_buffer_view(buffer, &view) == UMI_STATUS_OK);
    assert(strcmp(view.bytes, "xreturn") == 0);

    revision = view.revision;
    assert(umi_editor_inline_suggestion_session_accept(
               session,
               buffer,
               revision,
               UMI_EDITOR_INLINE_SUGGESTION_ACCEPT_ALL,
               &acceptance) == UMI_STATUS_OK);
    assert(acceptance.complete);
    assert(strcmp(acceptance.inserted_text, " value;") == 0);
    assert(umi_editor_text_buffer_view(buffer, &view) == UMI_STATUS_OK);
    assert(strcmp(view.bytes, "xreturn value;") == 0);

    umi_editor_inline_suggestion_session_destroy(session);
    session = NULL;
    assert(umi_editor_inline_suggestion_session_create(registry, &session) ==
           UMI_STATUS_OK);
    policy.allow_ai = 0;
    assert(umi_editor_inline_suggestion_session_set_policy(session, &policy) ==
           UMI_STATUS_OK);
    request.request_id = 82U;
    request.document_revision = view.revision;
    request.cursor_offset = view.byte_count;
    local.text = "hello";
    local.streaming = 0;
    assert(umi_editor_inline_suggestion_session_begin(session, &request) ==
           UMI_STATUS_OK);
    assert(umi_editor_inline_suggestion_session_typed_through(session,
                                                               "he",
                                                               request.document_revision + 1U) ==
           UMI_STATUS_OK);
    assert(umi_editor_inline_suggestion_session_current(session, &current) ==
           UMI_STATUS_OK);
    assert(strcmp(current.text, "llo") == 0);
    assert(umi_editor_inline_suggestion_session_reject(session) ==
           UMI_STATUS_OK);

    umi_editor_inline_suggestion_session_destroy(session);
    umi_editor_inline_suggestion_provider_registry_destroy(registry);
    umi_editor_text_buffer_destroy(buffer);
    return 0;
}
