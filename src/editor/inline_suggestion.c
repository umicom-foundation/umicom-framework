/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/inline_suggestion.c
 *
 * PURPOSE:
 *   Implement inline suggestion collections and provider registration,
 *   capability selection, invocation, streaming metadata and cancellation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/inline_suggestion.h"

#include <stdlib.h>
#include <string.h>

struct UmiEditorInlineSuggestionCollection {
    UmiEditorInlineSuggestion *suggestions;
    size_t count;
    size_t capacity;
    uint64_t revision;
};

typedef struct InlineProviderEntry {
    UmiEditorInlineSuggestionProviderDescriptor descriptor;
    UmiEditorInlineSuggestionProviderCallbacks callbacks;
    void *user_data;
} InlineProviderEntry;

struct UmiEditorInlineSuggestionProviderRegistry {
    InlineProviderEntry *entries;
    size_t count;
    size_t capacity;
    uint64_t revision;
};

/* Provide the next revision operation used by this module and its client applications. */
static uint64_t next_revision(uint64_t revision)
{
    return revision == UINT64_MAX ? 1U : revision + 1U;
}

/* Provide the terminated operation used by this module and its client applications. */
static int terminated(const char *text, size_t capacity)
{
    return text != NULL && memchr(text, '\0', capacity) != NULL;
}

/*
 * Provide the reserve suggestions operation used by this module and its client
 * applications.
 */
static UmiStatus reserve_suggestions(
    UmiEditorInlineSuggestionCollection *collection,
    size_t required)
{
    size_t capacity;
    UmiEditorInlineSuggestion *replacement;

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (required <= collection->capacity) return UMI_STATUS_OK;
    capacity = collection->capacity > 0U ? collection->capacity : 8U;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (capacity < required) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (capacity > SIZE_MAX / 2U) return UMI_STATUS_CAPACITY_EXCEEDED;
        capacity *= 2U;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (capacity > SIZE_MAX / sizeof(*replacement)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    replacement = (UmiEditorInlineSuggestion *)realloc(
        collection->suggestions, capacity * sizeof(*replacement));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    collection->suggestions = replacement;
    collection->capacity = capacity;
    return UMI_STATUS_OK;
}

/* Provide the find suggestion operation used by this module and its client applications. */
static size_t find_suggestion(
    const UmiEditorInlineSuggestionCollection *collection,
    const char *provider_id,
    const char *suggestion_id)
{
    size_t position;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (collection == NULL || provider_id == NULL || suggestion_id == NULL) {
        return SIZE_MAX;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (position = 0U; position < collection->count; ++position) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(collection->suggestions[position].provider_id,
                   provider_id) == 0 &&
            strcmp(collection->suggestions[position].id, suggestion_id) == 0) {
            return position;
        }
    }
    return SIZE_MAX;
}

/*
 * Check that editor inline suggestion satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_editor_inline_suggestion_validate(
    const UmiEditorInlineSuggestion *suggestion)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (suggestion == NULL ||
        suggestion->struct_size != (uint32_t)sizeof(*suggestion) ||
        suggestion->api_version != UMI_EDITOR_INLINE_SUGGESTION_API_VERSION ||
        !terminated(suggestion->id, sizeof(suggestion->id)) ||
        suggestion->id[0] == '\0' ||
        !terminated(suggestion->provider_id,
                    sizeof(suggestion->provider_id)) ||
        suggestion->provider_id[0] == '\0' ||
        !terminated(suggestion->document_id,
                    sizeof(suggestion->document_id)) ||
        !terminated(suggestion->label, sizeof(suggestion->label)) ||
        !terminated(suggestion->text, sizeof(suggestion->text)) ||
        suggestion->text[0] == '\0' ||
        !terminated(suggestion->prefix_context,
                    sizeof(suggestion->prefix_context)) ||
        !terminated(suggestion->suffix_context,
                    sizeof(suggestion->suffix_context)) ||
        suggestion->replace_end_offset < suggestion->replace_start_offset ||
        suggestion->confidence_milli < 0 ||
        suggestion->confidence_milli > 1000 ||
        suggestion->source < UMI_EDITOR_COMPLETION_SOURCE_OTHER ||
        suggestion->source > UMI_EDITOR_COMPLETION_SOURCE_AI ||
        (suggestion->flags &
         ~(UmiEditorInlineSuggestionFlags)
             UMI_EDITOR_INLINE_SUGGESTION_ALL_FLAGS) != 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the normalise suggestion operation used by this module and its client
 * applications.
 */
static UmiEditorInlineSuggestion normalise_suggestion(
    const UmiEditorInlineSuggestion *suggestion)
{
    UmiEditorInlineSuggestion stored = *suggestion;

    /* Apply this branch only when its contract condition is satisfied. */
    if (stored.source == UMI_EDITOR_COMPLETION_SOURCE_AI) {
        stored.flags |= UMI_EDITOR_INLINE_SUGGESTION_AI;
    }
    return stored;
}

/*
 * Initialise editor inline suggestion collection from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_inline_suggestion_collection_create(
    UmiEditorInlineSuggestionCollection **out_collection)
{
    UmiEditorInlineSuggestionCollection *collection;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_collection == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_collection = NULL;
    collection = (UmiEditorInlineSuggestionCollection *)calloc(
        1U, sizeof(*collection));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (collection == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    collection->revision = 1U;
    *out_collection = collection;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by editor inline suggestion collection so the same storage
 * can be reused safely.
 */
void umi_editor_inline_suggestion_collection_destroy(
    UmiEditorInlineSuggestionCollection *collection)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (collection == NULL) return;
    free(collection->suggestions);
    collection->suggestions = NULL;
    free(collection);
}

/*
 * Release or reset state held by editor inline suggestion collection so the same storage
 * can be reused safely.
 */
UmiStatus umi_editor_inline_suggestion_collection_clear(
    UmiEditorInlineSuggestionCollection *collection)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (collection == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    collection->count = 0U;
    collection->revision = next_revision(collection->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor inline suggestion collection upsert operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_inline_suggestion_collection_upsert(
    UmiEditorInlineSuggestionCollection *collection,
    const UmiEditorInlineSuggestion *suggestion)
{
    size_t position;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (collection == NULL ||
        umi_editor_inline_suggestion_validate(suggestion) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = find_suggestion(collection,
                               suggestion->provider_id,
                               suggestion->id);
    /* Apply this branch only when its contract condition is satisfied. */
    if (position == SIZE_MAX) {
        status = reserve_suggestions(collection, collection->count + 1U);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        position = collection->count++;
    }
    collection->suggestions[position] = normalise_suggestion(suggestion);
    collection->revision = next_revision(collection->revision);
    return UMI_STATUS_OK;
}

/*
 * Find editor inline suggestion collection while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_editor_inline_suggestion_collection_find(
    const UmiEditorInlineSuggestionCollection *collection,
    const char *provider_id,
    const char *suggestion_id,
    UmiEditorInlineSuggestion *out_suggestion)
{
    size_t position;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (collection == NULL || provider_id == NULL || suggestion_id == NULL ||
        out_suggestion == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = find_suggestion(collection, provider_id, suggestion_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (position == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_suggestion = collection->suggestions[position];
    return UMI_STATUS_OK;
}

/*
 * Find editor inline suggestion collection while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_editor_inline_suggestion_collection_at(
    const UmiEditorInlineSuggestionCollection *collection,
    size_t position,
    UmiEditorInlineSuggestion *out_suggestion)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (collection == NULL || out_suggestion == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (position >= collection->count) return UMI_STATUS_NOT_FOUND;
    *out_suggestion = collection->suggestions[position];
    return UMI_STATUS_OK;
}

/*
 * Provide the editor inline suggestion collection snapshot operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_inline_suggestion_collection_snapshot(
    const UmiEditorInlineSuggestionCollection *collection,
    UmiEditorInlineSuggestionCollectionSnapshot *out_snapshot)
{
    size_t position;
    size_t comparison;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (collection == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_EDITOR_INLINE_SUGGESTION_API_VERSION;
    out_snapshot->suggestion_count = collection->count;
    out_snapshot->revision = collection->revision;
    /* Visit each bounded item once so every record receives the same rule. */
    for (position = 0U; position < collection->count; ++position) {
        const UmiEditorInlineSuggestion *suggestion =
            &collection->suggestions[position];
        int first_provider = 1;
        /* Visit each bounded item once so every record receives the same rule. */
        for (comparison = 0U; comparison < position; ++comparison) {
            /* Use the stable identifier comparison to choose the matching record or policy. */
            if (strcmp(suggestion->provider_id,
                       collection->suggestions[comparison].provider_id) == 0) {
                first_provider = 0;
                break;
            }
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (first_provider) ++out_snapshot->provider_count;
        /* Apply this branch only when its contract condition is satisfied. */
        if ((suggestion->flags & UMI_EDITOR_INLINE_SUGGESTION_AI) != 0U) {
            ++out_snapshot->ai_count;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if ((suggestion->flags & UMI_EDITOR_INLINE_SUGGESTION_REMOTE) != 0U) {
            ++out_snapshot->remote_count;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if ((suggestion->flags &
             UMI_EDITOR_INLINE_SUGGESTION_STREAMING) != 0U) {
            ++out_snapshot->streaming_count;
        }
    }
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by editor inline suggestion collection without
 * changing their state.
 */
size_t umi_editor_inline_suggestion_collection_count(
    const UmiEditorInlineSuggestionCollection *collection)
{
    return collection != NULL ? collection->count : 0U;
}

/*
 * Provide the editor inline suggestion collection revision operation used by this module
 * and its client applications.
 */
uint64_t umi_editor_inline_suggestion_collection_revision(
    const UmiEditorInlineSuggestionCollection *collection)
{
    return collection != NULL ? collection->revision : 0U;
}

/*
 * Provide the editor inline suggestion accepts typed prefix operation used by this module
 * and its client applications.
 */
int umi_editor_inline_suggestion_accepts_typed_prefix(
    const UmiEditorInlineSuggestion *suggestion,
    const char *typed_text)
{
    const size_t typed_length = typed_text != NULL ? strlen(typed_text) : 0U;

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_editor_inline_suggestion_validate(suggestion) != UMI_STATUS_OK ||
        typed_text == NULL || typed_length == 0U) {
        return 0;
    }
    return strncmp(suggestion->text, typed_text, typed_length) == 0;
}

/*
 * Provide the validate provider descriptor operation used by this module and its client
 * applications.
 */
static UmiStatus validate_provider_descriptor(
    const UmiEditorInlineSuggestionProviderDescriptor *descriptor)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (descriptor == NULL ||
        descriptor->struct_size != (uint32_t)sizeof(*descriptor) ||
        descriptor->api_version != UMI_EDITOR_INLINE_SUGGESTION_API_VERSION ||
        !terminated(descriptor->id, sizeof(descriptor->id)) ||
        descriptor->id[0] == '\0' ||
        !terminated(descriptor->label, sizeof(descriptor->label)) ||
        descriptor->label[0] == '\0' ||
        !terminated(descriptor->language_id,
                    sizeof(descriptor->language_id)) ||
        descriptor->language_id[0] == '\0' ||
        descriptor->source < UMI_EDITOR_COMPLETION_SOURCE_OTHER ||
        descriptor->source > UMI_EDITOR_COMPLETION_SOURCE_AI) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the validate provider callbacks operation used by this module and its client
 * applications.
 */
static UmiStatus validate_provider_callbacks(
    const UmiEditorInlineSuggestionProviderCallbacks *callbacks)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (callbacks == NULL ||
        callbacks->struct_size != (uint32_t)sizeof(*callbacks) ||
        callbacks->api_version != UMI_EDITOR_INLINE_SUGGESTION_API_VERSION ||
        callbacks->provide == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the reserve provider entries operation used by this module and its client
 * applications.
 */
static UmiStatus reserve_provider_entries(
    UmiEditorInlineSuggestionProviderRegistry *registry,
    size_t required)
{
    size_t capacity;
    InlineProviderEntry *replacement;

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (required <= registry->capacity) return UMI_STATUS_OK;
    capacity = registry->capacity > 0U ? registry->capacity : 8U;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (capacity < required) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (capacity > SIZE_MAX / 2U) return UMI_STATUS_CAPACITY_EXCEEDED;
        capacity *= 2U;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (capacity > SIZE_MAX / sizeof(*replacement)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    replacement = (InlineProviderEntry *)realloc(
        registry->entries, capacity * sizeof(*replacement));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    registry->entries = replacement;
    registry->capacity = capacity;
    return UMI_STATUS_OK;
}

/*
 * Provide the find provider entry operation used by this module and its client
 * applications.
 */
static size_t find_provider_entry(
    const UmiEditorInlineSuggestionProviderRegistry *registry,
    const char *provider_id)
{
    size_t position;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || provider_id == NULL) return SIZE_MAX;
    /* Visit each bounded item once so every record receives the same rule. */
    for (position = 0U; position < registry->count; ++position) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(registry->entries[position].descriptor.id, provider_id) == 0) {
            return position;
        }
    }
    return SIZE_MAX;
}

/*
 * Initialise editor inline suggestion provider registry from caller-provided values so
 * later operations receive a known state.
 */
UmiStatus umi_editor_inline_suggestion_provider_registry_create(
    UmiEditorInlineSuggestionProviderRegistry **out_registry)
{
    UmiEditorInlineSuggestionProviderRegistry *registry;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;
    registry = (UmiEditorInlineSuggestionProviderRegistry *)calloc(
        1U, sizeof(*registry));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    registry->revision = 1U;
    *out_registry = registry;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by editor inline suggestion provider registry so the same
 * storage can be reused safely.
 */
void umi_editor_inline_suggestion_provider_registry_destroy(
    UmiEditorInlineSuggestionProviderRegistry *registry)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) return;
    free(registry->entries);
    registry->entries = NULL;
    free(registry);
}

/*
 * Add editor inline suggestion provider registry only after its inputs and available
 * capacity have been checked.
 */
UmiStatus umi_editor_inline_suggestion_provider_registry_register(
    UmiEditorInlineSuggestionProviderRegistry *registry,
    const UmiEditorInlineSuggestionProviderDescriptor *descriptor,
    const UmiEditorInlineSuggestionProviderCallbacks *callbacks,
    void *provider_user_data)
{
    InlineProviderEntry entry;
    size_t position;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL ||
        validate_provider_descriptor(descriptor) != UMI_STATUS_OK ||
        validate_provider_callbacks(callbacks) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = find_provider_entry(registry, descriptor->id);
    /* Apply this branch only when its contract condition is satisfied. */
    if (position == SIZE_MAX) {
        status = reserve_provider_entries(registry, registry->count + 1U);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        position = registry->count++;
    }
    (void)memset(&entry, 0, sizeof(entry));
    entry.descriptor = *descriptor;
    entry.descriptor.supports_manual_invocation =
        descriptor->supports_manual_invocation != 0;
    entry.descriptor.supports_automatic_invocation =
        descriptor->supports_automatic_invocation != 0;
    entry.descriptor.supports_streaming = descriptor->supports_streaming != 0;
    entry.descriptor.supports_partial_acceptance =
        descriptor->supports_partial_acceptance != 0;
    entry.descriptor.requires_network = descriptor->requires_network != 0;
    entry.descriptor.requires_workspace_trust =
        descriptor->requires_workspace_trust != 0;
    entry.descriptor.enabled = descriptor->enabled != 0;
    entry.callbacks = *callbacks;
    entry.user_data = provider_user_data;
    registry->entries[position] = entry;
    registry->revision = next_revision(registry->revision);
    return UMI_STATUS_OK;
}

/*
 * Remove editor inline suggestion provider registry while keeping the remaining records in
 * a valid and discoverable state.
 */
UmiStatus umi_editor_inline_suggestion_provider_registry_unregister(
    UmiEditorInlineSuggestionProviderRegistry *registry,
    const char *provider_id)
{
    size_t position;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || provider_id == NULL || provider_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = find_provider_entry(registry, provider_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (position == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (position + 1U < registry->count) {
        (void)memmove(&registry->entries[position],
                      &registry->entries[position + 1U],
                      (registry->count - position - 1U) *
                          sizeof(*registry->entries));
    }
    --registry->count;
    registry->revision = next_revision(registry->revision);
    return UMI_STATUS_OK;
}

/*
 * Find editor inline suggestion provider registry while leaving the underlying catalogue
 * or model owned by this module.
 */
UmiStatus umi_editor_inline_suggestion_provider_registry_at(
    const UmiEditorInlineSuggestionProviderRegistry *registry,
    size_t position,
    UmiEditorInlineSuggestionProviderDescriptor *out_descriptor)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || out_descriptor == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (position >= registry->count) return UMI_STATUS_NOT_FOUND;
    *out_descriptor = registry->entries[position].descriptor;
    return UMI_STATUS_OK;
}

/*
 * Find editor inline suggestion provider registry while leaving the underlying catalogue
 * or model owned by this module.
 */
UmiStatus umi_editor_inline_suggestion_provider_registry_find(
    const UmiEditorInlineSuggestionProviderRegistry *registry,
    const char *provider_id,
    UmiEditorInlineSuggestionProviderDescriptor *out_descriptor)
{
    size_t position;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || provider_id == NULL || out_descriptor == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = find_provider_entry(registry, provider_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (position == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_descriptor = registry->entries[position].descriptor;
    return UMI_STATUS_OK;
}

/*
 * Provide the editor inline suggestion provider supports request operation used by this
 * module and its client applications.
 */
int umi_editor_inline_suggestion_provider_supports_request(
    const UmiEditorInlineSuggestionProviderDescriptor *descriptor,
    const UmiEditorCompletionRequest *request)
{
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (validate_provider_descriptor(descriptor) != UMI_STATUS_OK ||
        umi_editor_completion_request_validate(request) != UMI_STATUS_OK ||
        !descriptor->enabled ||
        (strcmp(descriptor->language_id, "*") != 0 &&
         strcmp(descriptor->language_id, request->language_id) != 0)) {
        return 0;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (request->trigger_kind == UMI_EDITOR_COMPLETION_TRIGGER_INVOKED) {
        return descriptor->supports_manual_invocation;
    }
    return descriptor->supports_automatic_invocation;
}

/*
 * Provide the editor inline suggestion provider registry invoke operation used by this
 * module and its client applications.
 */
UmiStatus umi_editor_inline_suggestion_provider_registry_invoke(
    const UmiEditorInlineSuggestionProviderRegistry *registry,
    const char *provider_id,
    const UmiEditorCompletionRequest *request,
    UmiEditorInlineSuggestionSink sink,
    void *sink_user_data,
    UmiEditorInlineSuggestionProviderResponse *out_response)
{
    size_t position;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || provider_id == NULL || sink == NULL ||
        out_response == NULL ||
        umi_editor_completion_request_validate(request) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = find_provider_entry(registry, provider_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (position == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!umi_editor_inline_suggestion_provider_supports_request(
            &registry->entries[position].descriptor, request)) {
        return UMI_STATUS_UNAVAILABLE;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_editor_completion_request_is_cancelled(request)) {
        return UMI_STATUS_CANCELLED;
    }
    (void)memset(out_response, 0, sizeof(*out_response));
    out_response->struct_size = (uint32_t)sizeof(*out_response);
    out_response->api_version = UMI_EDITOR_INLINE_SUGGESTION_API_VERSION;
    return registry->entries[position].callbacks.provide(
        request,
        sink,
        sink_user_data,
        out_response,
        registry->entries[position].user_data);
}

/*
 * Provide the editor inline suggestion provider registry cancel operation used by this
 * module and its client applications.
 */
UmiStatus umi_editor_inline_suggestion_provider_registry_cancel(
    const UmiEditorInlineSuggestionProviderRegistry *registry,
    const char *provider_id,
    uint64_t request_id)
{
    size_t position;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || provider_id == NULL || provider_id[0] == '\0' ||
        request_id == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = find_provider_entry(registry, provider_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (position == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry->entries[position].callbacks.cancel == NULL) {
        return UMI_STATUS_NOT_IMPLEMENTED;
    }
    return registry->entries[position].callbacks.cancel(
        request_id, registry->entries[position].user_data);
}

/*
 * Provide the editor inline suggestion provider registry snapshot operation used by this
 * module and its client applications.
 */
UmiStatus umi_editor_inline_suggestion_provider_registry_snapshot(
    const UmiEditorInlineSuggestionProviderRegistry *registry,
    UmiEditorInlineSuggestionProviderRegistrySnapshot *out_snapshot)
{
    size_t position;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_EDITOR_INLINE_SUGGESTION_API_VERSION;
    out_snapshot->provider_count = registry->count;
    out_snapshot->revision = registry->revision;
    /* Visit each bounded item once so every record receives the same rule. */
    for (position = 0U; position < registry->count; ++position) {
        const UmiEditorInlineSuggestionProviderDescriptor *descriptor =
            &registry->entries[position].descriptor;
        /* Apply this operation only while the related capability or state is available. */
        if (descriptor->enabled) ++out_snapshot->enabled_provider_count;
        /* Apply this branch only when its contract condition is satisfied. */
        if (descriptor->supports_streaming) {
            ++out_snapshot->streaming_provider_count;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (descriptor->source == UMI_EDITOR_COMPLETION_SOURCE_AI) {
            ++out_snapshot->ai_provider_count;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (descriptor->requires_network) {
            ++out_snapshot->remote_provider_count;
        }
    }
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by editor inline suggestion provider registry
 * without changing their state.
 */
size_t umi_editor_inline_suggestion_provider_registry_count(
    const UmiEditorInlineSuggestionProviderRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}

/*
 * Provide the editor inline suggestion provider registry revision operation used by this
 * module and its client applications.
 */
uint64_t umi_editor_inline_suggestion_provider_registry_revision(
    const UmiEditorInlineSuggestionProviderRegistry *registry)
{
    return registry != NULL ? registry->revision : 0U;
}
