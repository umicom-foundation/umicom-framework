/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/inline_suggestion.h
 *
 * PURPOSE:
 *   Define ghost-text suggestions, provider capabilities and registries for
 *   deterministic local, Language Server and governed AI inline completion.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_INLINE_SUGGESTION_H
#define UMICOM_EDITOR_INLINE_SUGGESTION_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/editor/completion_provider.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDITOR_INLINE_SUGGESTION_API_VERSION 1U
#define UMI_EDITOR_INLINE_SUGGESTION_TEXT_CAPACITY 4096U
#define UMI_EDITOR_INLINE_SUGGESTION_CONTEXT_CAPACITY 512U
#define UMI_EDITOR_INLINE_SUGGESTION_LABEL_CAPACITY 256U

typedef uint32_t UmiEditorInlineSuggestionFlags;
enum {
    UMI_EDITOR_INLINE_SUGGESTION_AI = 1U << 0,
    UMI_EDITOR_INLINE_SUGGESTION_REMOTE = 1U << 1,
    UMI_EDITOR_INLINE_SUGGESTION_STREAMING = 1U << 2,
    UMI_EDITOR_INLINE_SUGGESTION_COMPLETE = 1U << 3,
    UMI_EDITOR_INLINE_SUGGESTION_REQUIRES_TRUST = 1U << 4,
    UMI_EDITOR_INLINE_SUGGESTION_REQUIRES_CONFIRMATION = 1U << 5,
    UMI_EDITOR_INLINE_SUGGESTION_ALL_FLAGS = (1U << 6) - 1U
};

/**
 * Represent the editor inline suggestion data shared with callers of this public contract.
 */
typedef struct UmiEditorInlineSuggestion {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char provider_id[UMI_EDITOR_COMPLETION_PROVIDER_ID_CAPACITY];
    char document_id[UMI_EDITOR_COMPLETION_DOCUMENT_ID_CAPACITY];
    char label[UMI_EDITOR_INLINE_SUGGESTION_LABEL_CAPACITY];
    char text[UMI_EDITOR_INLINE_SUGGESTION_TEXT_CAPACITY];
    char prefix_context[UMI_EDITOR_INLINE_SUGGESTION_CONTEXT_CAPACITY];
    char suffix_context[UMI_EDITOR_INLINE_SUGGESTION_CONTEXT_CAPACITY];
    uint64_t replace_start_offset;
    uint64_t replace_end_offset;
    uint64_t document_revision;
    uint64_t request_id;
    int32_t provider_priority;
    int32_t confidence_milli;
    UmiEditorCompletionSource source;
    UmiEditorInlineSuggestionFlags flags;
} UmiEditorInlineSuggestion;

/**
 * Represent the editor inline suggestion collection snapshot data shared with callers of
 * this public contract.
 */
typedef struct UmiEditorInlineSuggestionCollectionSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    size_t suggestion_count;
    size_t provider_count;
    size_t ai_count;
    size_t remote_count;
    size_t streaming_count;
    uint64_t revision;
} UmiEditorInlineSuggestionCollectionSnapshot;

/**
 * Represent the editor inline suggestion collection data shared with callers of this
 * public contract.
 */
typedef struct UmiEditorInlineSuggestionCollection
    UmiEditorInlineSuggestionCollection;

/**
 * Initialise editor inline suggestion collection from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_inline_suggestion_collection_create(
    UmiEditorInlineSuggestionCollection **out_collection);
/**
 * Release or reset state held by editor inline suggestion collection so the same storage
 * can be reused safely.
 */
void umi_editor_inline_suggestion_collection_destroy(
    UmiEditorInlineSuggestionCollection *collection);
/**
 * Release or reset state held by editor inline suggestion collection so the same storage
 * can be reused safely.
 */
UmiStatus umi_editor_inline_suggestion_collection_clear(
    UmiEditorInlineSuggestionCollection *collection);
/**
 * Provide the editor inline suggestion collection upsert operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_inline_suggestion_collection_upsert(
    UmiEditorInlineSuggestionCollection *collection,
    const UmiEditorInlineSuggestion *suggestion);
/**
 * Find editor inline suggestion collection while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_editor_inline_suggestion_collection_find(
    const UmiEditorInlineSuggestionCollection *collection,
    const char *provider_id,
    const char *suggestion_id,
    UmiEditorInlineSuggestion *out_suggestion);
/**
 * Find editor inline suggestion collection while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_editor_inline_suggestion_collection_at(
    const UmiEditorInlineSuggestionCollection *collection,
    size_t position,
    UmiEditorInlineSuggestion *out_suggestion);
/**
 * Provide the editor inline suggestion collection snapshot operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_inline_suggestion_collection_snapshot(
    const UmiEditorInlineSuggestionCollection *collection,
    UmiEditorInlineSuggestionCollectionSnapshot *out_snapshot);
/**
 * Return the number of records represented by editor inline suggestion collection without
 * changing their state.
 */
size_t umi_editor_inline_suggestion_collection_count(
    const UmiEditorInlineSuggestionCollection *collection);
/**
 * Provide the editor inline suggestion collection revision operation used by this module
 * and its client applications.
 */
uint64_t umi_editor_inline_suggestion_collection_revision(
    const UmiEditorInlineSuggestionCollection *collection);
/**
 * Check that editor inline suggestion satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_editor_inline_suggestion_validate(
    const UmiEditorInlineSuggestion *suggestion);
/**
 * Provide the editor inline suggestion accepts typed prefix operation used by this module
 * and its client applications.
 */
int umi_editor_inline_suggestion_accepts_typed_prefix(
    const UmiEditorInlineSuggestion *suggestion,
    const char *typed_text);

/**
 * Represent the editor inline suggestion provider descriptor data shared with callers of
 * this public contract.
 */
typedef struct UmiEditorInlineSuggestionProviderDescriptor {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_EDITOR_COMPLETION_PROVIDER_ID_CAPACITY];
    char label[UMI_EDITOR_COMPLETION_PROVIDER_LABEL_CAPACITY];
    char language_id[UMI_EDITOR_COMPLETION_LANGUAGE_ID_CAPACITY];
    int32_t priority;
    UmiEditorCompletionSource source;
    uint32_t debounce_milliseconds;
    uint32_t preferred_latency_milliseconds;
    int supports_manual_invocation;
    int supports_automatic_invocation;
    int supports_streaming;
    int supports_partial_acceptance;
    int requires_network;
    int requires_workspace_trust;
    int enabled;
} UmiEditorInlineSuggestionProviderDescriptor;

/**
 * Represent the editor inline suggestion provider response data shared with callers of
 * this public contract.
 */
typedef struct UmiEditorInlineSuggestionProviderResponse {
    uint32_t struct_size;
    uint32_t api_version;
    size_t emitted_suggestion_count;
    uint64_t provider_revision;
    int incomplete;
    int streaming;
} UmiEditorInlineSuggestionProviderResponse;

typedef UmiStatus (*UmiEditorInlineSuggestionSink)(
    const UmiEditorInlineSuggestion *suggestion,
    void *user_data);
typedef UmiStatus (*UmiEditorInlineSuggestionProvideFunction)(
    const UmiEditorCompletionRequest *request,
    UmiEditorInlineSuggestionSink sink,
    void *sink_user_data,
    UmiEditorInlineSuggestionProviderResponse *out_response,
    void *provider_user_data);
typedef UmiStatus (*UmiEditorInlineSuggestionCancelFunction)(
    uint64_t request_id,
    void *provider_user_data);

/**
 * Represent the editor inline suggestion provider callbacks data shared with callers of
 * this public contract.
 */
typedef struct UmiEditorInlineSuggestionProviderCallbacks {
    uint32_t struct_size;
    uint32_t api_version;
    UmiEditorInlineSuggestionProvideFunction provide;
    UmiEditorInlineSuggestionCancelFunction cancel;
} UmiEditorInlineSuggestionProviderCallbacks;

/**
 * Represent the editor inline suggestion provider registry snapshot data shared with
 * callers of this public contract.
 */
typedef struct UmiEditorInlineSuggestionProviderRegistrySnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    size_t provider_count;
    size_t enabled_provider_count;
    size_t streaming_provider_count;
    size_t ai_provider_count;
    size_t remote_provider_count;
    uint64_t revision;
} UmiEditorInlineSuggestionProviderRegistrySnapshot;

/**
 * Represent the editor inline suggestion provider registry data shared with callers of
 * this public contract.
 */
typedef struct UmiEditorInlineSuggestionProviderRegistry
    UmiEditorInlineSuggestionProviderRegistry;

/**
 * Initialise editor inline suggestion provider registry from caller-provided values so
 * later operations receive a known state.
 */
UmiStatus umi_editor_inline_suggestion_provider_registry_create(
    UmiEditorInlineSuggestionProviderRegistry **out_registry);
/**
 * Release or reset state held by editor inline suggestion provider registry so the same
 * storage can be reused safely.
 */
void umi_editor_inline_suggestion_provider_registry_destroy(
    UmiEditorInlineSuggestionProviderRegistry *registry);
/**
 * Add editor inline suggestion provider registry only after its inputs and available
 * capacity have been checked.
 */
UmiStatus umi_editor_inline_suggestion_provider_registry_register(
    UmiEditorInlineSuggestionProviderRegistry *registry,
    const UmiEditorInlineSuggestionProviderDescriptor *descriptor,
    const UmiEditorInlineSuggestionProviderCallbacks *callbacks,
    void *provider_user_data);
/**
 * Remove editor inline suggestion provider registry while keeping the remaining records in
 * a valid and discoverable state.
 */
UmiStatus umi_editor_inline_suggestion_provider_registry_unregister(
    UmiEditorInlineSuggestionProviderRegistry *registry,
    const char *provider_id);
/**
 * Find editor inline suggestion provider registry while leaving the underlying catalogue
 * or model owned by this module.
 */
UmiStatus umi_editor_inline_suggestion_provider_registry_at(
    const UmiEditorInlineSuggestionProviderRegistry *registry,
    size_t position,
    UmiEditorInlineSuggestionProviderDescriptor *out_descriptor);
/**
 * Find editor inline suggestion provider registry while leaving the underlying catalogue
 * or model owned by this module.
 */
UmiStatus umi_editor_inline_suggestion_provider_registry_find(
    const UmiEditorInlineSuggestionProviderRegistry *registry,
    const char *provider_id,
    UmiEditorInlineSuggestionProviderDescriptor *out_descriptor);
/**
 * Provide the editor inline suggestion provider registry snapshot operation used by this
 * module and its client applications.
 */
UmiStatus umi_editor_inline_suggestion_provider_registry_snapshot(
    const UmiEditorInlineSuggestionProviderRegistry *registry,
    UmiEditorInlineSuggestionProviderRegistrySnapshot *out_snapshot);
/**
 * Provide the editor inline suggestion provider supports request operation used by this
 * module and its client applications.
 */
int umi_editor_inline_suggestion_provider_supports_request(
    const UmiEditorInlineSuggestionProviderDescriptor *descriptor,
    const UmiEditorCompletionRequest *request);
/**
 * Provide the editor inline suggestion provider registry invoke operation used by this
 * module and its client applications.
 */
UmiStatus umi_editor_inline_suggestion_provider_registry_invoke(
    const UmiEditorInlineSuggestionProviderRegistry *registry,
    const char *provider_id,
    const UmiEditorCompletionRequest *request,
    UmiEditorInlineSuggestionSink sink,
    void *sink_user_data,
    UmiEditorInlineSuggestionProviderResponse *out_response);
/**
 * Provide the editor inline suggestion provider registry cancel operation used by this
 * module and its client applications.
 */
UmiStatus umi_editor_inline_suggestion_provider_registry_cancel(
    const UmiEditorInlineSuggestionProviderRegistry *registry,
    const char *provider_id,
    uint64_t request_id);
/**
 * Return the number of records represented by editor inline suggestion provider registry
 * without changing their state.
 */
size_t umi_editor_inline_suggestion_provider_registry_count(
    const UmiEditorInlineSuggestionProviderRegistry *registry);
/**
 * Provide the editor inline suggestion provider registry revision operation used by this
 * module and its client applications.
 */
uint64_t umi_editor_inline_suggestion_provider_registry_revision(
    const UmiEditorInlineSuggestionProviderRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_INLINE_SUGGESTION_H */
